import { EventEmitter } from "events";
import { loadBinding } from "./utils/paths";
import { PumpingService } from "./services/pumping";
import { TradesService } from "./services/trades";
import { UsersService } from "./services/users";
import { NativeMT4Manager } from "./native/types";

const nativeBinding = loadBinding();

export default class MT4Manager {
  private readonly native: NativeMT4Manager;
  public readonly users: UsersService;
  public readonly trades: TradesService;
  private pumpInstance?: PumpingService;

  constructor(config: { dllPath: string });
  constructor(dllPath: string);

  constructor(options: string | { dllPath: string }) {
    const config = typeof options === "string" ? { dllPath: options } : options;

    this.native = new nativeBinding.MT4Manager(
      config.dllPath,
    ) as NativeMT4Manager;
    this.users = new UsersService(this.native);
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

  startPumping() {
    return this.native.startPumping();
  }

  stopPumping() {
    this.native.stopPumping();
  }
}

export { MT4Manager };
