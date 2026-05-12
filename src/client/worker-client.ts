// src/client/worker-client.ts

import { EventEmitter } from "node:events";
import { Worker } from "node:worker_threads";

type PendingRequest = {
  resolve: (value: any) => void;
  reject: (reason?: any) => void;
  timeout?: NodeJS.Timeout;
};

export type RpcResponseMessage = {
  id: number;
  result?: any;
  error?: {
    name?: string;
    message: string;
    code?: string;
    stack?: string;
  };
};

export type EventMessage = {
  kind: "event";
  scope: string;
  event: string;
  data: any;
};

export type RpcMessage = RpcResponseMessage | EventMessage;

export class WorkerClient extends EventEmitter {
  private seq = 0;

  private pending = new Map<number, PendingRequest>();

  constructor(private readonly worker: Worker) {
    super();

    this.bindWorkerEvents();
  }

  // ---------------------------------------------------------
  // RPC
  // ---------------------------------------------------------

  call<T = any>(method: string, params?: any, timeoutMs = 30000): Promise<T> {
    const id = ++this.seq;

    return new Promise<T>((resolve, reject) => {
      const timeout = setTimeout(() => {
        this.pending.delete(id);

        reject(
          Object.assign(new Error(`Worker call timeout: ${method}`), {
            code: "WORKER_TIMEOUT",
          }),
        );
      }, timeoutMs);

      this.pending.set(id, {
        resolve,
        reject,
        timeout,
      });

      this.worker.postMessage({
        id,
        method,
        params,
      });
    });
  }

  // ---------------------------------------------------------
  // EVENTS
  // ---------------------------------------------------------

  private bindWorkerEvents() {
    this.worker.on("message", (msg: RpcMessage) => {
      /**
       * Pumping / native events
       */
      if ("kind" in msg && msg.kind === "event") {
        this.emit("event", msg);
        return;
      }

      if (!("id" in msg)) return;

      /**
       * RPC response
       */
      const request = this.pending.get(msg.id);

      if (!request) {
        return;
      }

      this.pending.delete(msg.id);

      if (request.timeout) {
        clearTimeout(request.timeout);
      }

      if (msg.error) {
        const error = Object.assign(new Error(msg.error.message), {
          name: msg.error.name,
          code: msg.error.code,
          stack: msg.error.stack,
        });

        request.reject(error);

        return;
      }

      request.resolve(msg.result);
    });

    this.worker.on("error", (err) => {
      this.rejectAll(err);

      this.emit("error", err);
    });

    this.worker.on("exit", (code) => {
      const error = Object.assign(
        new Error(`Worker exited with code ${code}`),
        {
          code: "WORKER_EXIT",
        },
      );

      this.rejectAll(error);

      this.emit("exit", code);
    });
  }

  // ---------------------------------------------------------
  // CLEANUP
  // ---------------------------------------------------------

  async terminate(): Promise<number> {
    this.removeAllListeners();

    this.rejectAll(
      Object.assign(new Error("Worker terminated"), {
        code: "WORKER_TERMINATED",
      }),
    );

    return this.worker.terminate();
  }

  private rejectAll(error: any) {
    for (const request of this.pending.values()) {
      if (request.timeout) {
        clearTimeout(request.timeout);
      }

      request.reject(error);
    }

    this.pending.clear();
  }
}
