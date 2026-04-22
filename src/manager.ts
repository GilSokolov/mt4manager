import { NativeMT4Manager } from "./native/types";
import { PumpingService } from "./services/pumping";
import { TradesService } from "./services/trades";
import { UsersService } from "./services/users";
import { PumpingOptions } from "./types/misc";
import { loadBinding } from "./utils/paths";

const nativeBinding = loadBinding();

export default class MT4Manager {
  private readonly native: NativeMT4Manager;
  public readonly users: UsersService;
  public readonly trades: TradesService;
  private pumpInstance?: PumpingService;

  #pumping = false;
  #pumpConfig: PumpingOptions = normalizePumpingOptions();

  constructor(config: { dllPath: string });
  constructor(dllPath: string);

  constructor(options: string | { dllPath: string }) {
    const config = typeof options === "string" ? { dllPath: options } : options;

    this.native = new nativeBinding.MT4Manager(
      config.dllPath,
    ) as NativeMT4Manager;
    this.users = new UsersService(this.native.users);
    this.trades = new TradesService(this.native);
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
    if (this.pumpInstance) {
      this.pumpInstance.removeAllListeners();
      this.pumpInstance = undefined;
    }
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
    users: options.users ?? false,
  };
}
