import { EventEmitter } from "node:events";

import { EventMessage, WorkerClient } from "./worker-client";
import { Listener } from "../types";

export class Client<
  Type,
  Events extends string,
  Key extends keyof Type,
> extends EventEmitter {
  private watches = new Map<string, Set<Listener<Type>>>();

  constructor(
    private key: Key,
    private scope: string,
    public readonly worker: WorkerClient,
  ) {
    super();

    this.bindWorkerEvents();
  }

  // ---------------------------------------------------------
  // EVENTS
  // ---------------------------------------------------------

  on(eventName: Events, listener: (data: Type) => void): this {
    return super.on(eventName, listener);
  }

  once(eventName: Events, listener: (data: Type) => void): this {
    return super.once(eventName, listener);
  }

  private bindWorkerEvents() {
    this.worker.on("event", (msg: EventMessage) => {
      if (msg.scope !== this.scope) {
        return;
      }

      const data = msg.data as Type;
      const event = msg.event as Events;

      this.emit(event, data);

      this.dispatchWatch(data);
    });
  }

  // ---------------------------------------------------------
  // WATCHERS
  // ---------------------------------------------------------

  watch(target: Type[Key], listener: Listener<Type>) {
    const key = String(target);
    const set = this.watches.get(key) ?? new Set<Listener<Type>>();

    set.add(listener);

    this.watches.set(key, set);

    return () => {
      this.unwatch(key, listener);
    };
  }

  unwatch(target: string, listener?: Listener<Type>) {
    if (!listener) {
      this.watches.delete(target);
      return;
    }

    const set = this.watches.get(target);

    if (!set) {
      return;
    }

    set.delete(listener);

    if (set.size === 0) {
      this.watches.delete(target);
    }
  }

  unwatchAll() {
    this.watches.clear();
  }

  private dispatchWatch(data: Type) {
    const listeners = this.watches.get(String(data[this.key]));

    if (!listeners) {
      return;
    }

    for (const listener of listeners) {
      listener(data);
    }
  }
}
