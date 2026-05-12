import { EventEmitter } from "node:events";
import { Worker } from "node:worker_threads";

import { UsersClient } from "./services/users-client";
import { PositionsClient } from "./services/positions-client";
import { SymbolsClient } from "./services/symbols-client";
import { TransactionsClient } from "./services/transactions-client";

import { WorkerClient } from "./worker-client";

import { ManagerConfig, PumpingOptions } from "../types/manager";

import { normalizePumpingOptions } from "../utils/normalize-pumping-options";
import path from "node:path";
import fs from "node:fs";

const workerPath = path.resolve(__dirname, "../worker/mt4.worker.js");

export class MT4Manager extends EventEmitter {
  private readonly worker: WorkerClient;

  public readonly users: UsersClient;
  public readonly positions: PositionsClient;
  public readonly symbols: SymbolsClient;
  public readonly transactions: TransactionsClient;

  #pumping = false;
  #pumpConfig: PumpingOptions = normalizePumpingOptions();

  constructor(config: ManagerConfig);
  constructor(dllPath: string);

  constructor(options: string | ManagerConfig) {
    super();

    const workerThread = new Worker(workerPath, {
      workerData: { config: this.normalizeConfig(options) },
    });

    this.worker = new WorkerClient(workerThread);

    this.users = new UsersClient(this.worker);
    this.positions = new PositionsClient(this.worker);
    this.symbols = new SymbolsClient(this.worker);
    this.transactions = new TransactionsClient(this.worker);

    this.bindWorkerEvents();
  }

  // ---------------------------------------------------------
  // CONNECTION
  // ---------------------------------------------------------

  async connect(server: string): Promise<void> {
    return this.worker.call("manager.connect", {
      server,
    });
  }

  async login(login: number, password: string): Promise<void> {
    return this.worker.call("manager.login", {
      login,
      password,
    });
  }

  async disconnect(): Promise<void> {
    return this.worker.call("manager.disconnect");
  }

  async close(): Promise<void> {
    try {
      await this.stopPumping();
    } catch {
      // ignore
    }

    await this.worker.call("manager.close");

    this.users.unwatchAll();
    this.positions.unwatchAll();
    this.symbols.unwatchAll();
    this.transactions.unwatchAll();

    this.removeAllListeners();

    await this.worker.terminate();
  }

  // ---------------------------------------------------------
  // PUMPING
  // ---------------------------------------------------------

  async startPumping(options?: Partial<PumpingOptions>): Promise<void> {
    if (this.#pumping) {
      throw new Error("Pumping is already started");
    }

    const normalized = normalizePumpingOptions(options);

    await this.worker.call("pumping.start", {
      options: normalized,
    });

    this.#pumpConfig = normalized;
    this.#pumping = true;
  }

  async stopPumping(): Promise<void> {
    if (!this.#pumping) {
      return;
    }

    await this.worker.call("pumping.stop");

    this.#pumping = false;
  }

  async restartPumping(options?: Partial<PumpingOptions>): Promise<void> {
    const normalized = normalizePumpingOptions(options);

    if (this.#pumping) {
      await this.stopPumping();
    }

    await this.startPumping(normalized);
  }

  isPumping(): boolean {
    return this.#pumping;
  }

  getPumpingConfig(): PumpingOptions {
    return { ...this.#pumpConfig };
  }

  // ---------------------------------------------------------
  // EVENTS
  // ---------------------------------------------------------

  private bindWorkerEvents() {
    this.worker.on("event", (msg) => {
      /**
       * Optional:
       * also emit namespaced payload
       *
       * Example:
       * manager.on("users:update", ...)
       */
      this.emit(`${msg.scope}:${msg.event}`, msg.data);
    });

    this.worker.on("error", (err) => {
      this.emit("error", err);
    });

    this.worker.on("exit", (code) => {
      this.emit("exit", code);
    });
  }

  private normalizeConfig(options: string | ManagerConfig): ManagerConfig {
    if (!options) {
      throw new TypeError("Config is missing");
    }

    const config = typeof options === "string" ? { dllPath: options } : options;

    if (!config.dllPath) {
      throw new TypeError("Expected string: dllPath");
    }

    if (!config.dllPath.trim()) {
      throw new TypeError("Expected non-empty string: dllPath");
    }

    if (!fs.existsSync(config.dllPath)) {
      throw new Error("MT4 DLL path does not exist");
    }

    return config;
  }
}
