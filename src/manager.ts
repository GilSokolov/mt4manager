/*import { Positions } from "./services/positions";
import { Symbols } from "./services/symbols";
import { Transactions } from "./services/transactions";
import { Users } from "./services/users";
import { TradeRecord, EventType } from "./types";
import {
  ManagerConfig,
  NativeMT4Manager,
  PumpingOptions,
} from "./types/manager";

import { loadBinding } from "./utils/paths";
import { toEventType } from "./utils/to-event-type";

const nativeBinding = loadBinding();

export default class MT4Manager {
  private readonly native: NativeMT4Manager;
  public readonly users: Users;
  public readonly symbols: Symbols;
  public readonly positions: Positions;
  public readonly transactions: Transactions;

  #pumping = false;
  #pumpConfig: PumpingOptions = normalizePumpingOptions();

  constructor(config: ManagerConfig);
  constructor(dllPath: string);

  constructor(options: string | ManagerConfig) {
    const config = typeof options === "string" ? { dllPath: options } : options;

    this.native = new nativeBinding.MT4Manager(config);
    this.users = new Users(this.native.users);
    this.symbols = new Symbols(this.native.symbols);
    this.positions = new Positions(this.native.trades);
    this.transactions = new Transactions(this.native.trades);

    this.native.trades._setUpdateHandler(
      (trade: TradeRecord, type: EventType) => {
        const event = toEventType(type);
        if (event !== "unknown") {
          this.positions.handleEvent(event, trade);
          this.transactions.handleEvent(event, trade);
        }

        if (event === "add" || event === "delete") {
          this.users.handleTradeUpdate(trade.login);
        }
      },
    );

    this.symbols.on("tick", (tick) => {
      this.positions.HandleBidAskUpdate(tick.symbol);
    });
  }

  async connect(server: string): Promise<void> {
    return this.native.connect(server);
  }

  async login(login: number, password: string): Promise<void> {
    return this.native.login(login, password);
  }

  async disconnect(): Promise<void> {
    return this.native.disconnect();
  }

  async close(): Promise<void> {
    return this.native.close();
  }

  async startPumping(options?: Partial<PumpingOptions>) {
    if (this.#pumping) {
      throw new Error("Pumping is already started");
    }
    const normalized = normalizePumpingOptions(options);
    await this.native.startPumping(normalized);
    this.#pumpConfig = normalized;
    this.#pumping = true;
  }

  async stopPumping() {
    if (!this.#pumping) {
      return;
    }

    await this.native.stopPumping();

    this.#pumping = false;
  }

  async restartPumping(options: PumpingOptions): Promise<void> {
    const normalized = normalizePumpingOptions(options);

    if (this.#pumping) {
      await this.stopPumping();
      await this.startPumping(normalized);
      return;
    }

    this.#pumpConfig = normalized;
  }

  isPumping(): boolean {
    return this.#pumping;
  }

  getPumpingConfig(): PumpingOptions {
    return { ...this.#pumpConfig };
  }
}

export { MT4Manager };

function normalizePumpingOptions(
  options: Partial<PumpingOptions> = {},
): PumpingOptions {
  return {
    ticks: options.ticks ?? false,
    news: options.news ?? false,
    fullNews: options.fullNews ?? false,
    mail: options.mail ?? false,
    onlineUsers: options.onlineUsers ?? false,
    users: options.users ?? true,
  };
}
*/
export { MT4Manager } from "./client/MT4Manager";
