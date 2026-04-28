import { EventEmitter } from "node:events";
import { Listener } from "../types/manager";
import { MT4Symbol, MT4SymbolConfig, NativeSymbolsApi } from "../types/symbol";

type SymbolListener = Listener<MT4Symbol>;

export class SymbolService extends EventEmitter {
  constructor(private readonly native: NativeSymbolsApi) {
    super();
    // this.native._setUpdateHandler((user) => {
    //   this.emit("update", user);
    //   this.dispatchWatch(user);
    // });
  }

  private watches = new Map<string, Set<SymbolListener>>();

  get(symbol: string) {
    return this.native.get(symbol);
  }

  getAll(): Promise<MT4SymbolConfig[]> {
    return Promise.resolve(this.native.getAll());
  }

  watch(target: string, listener: SymbolListener) {
    const set = this.watches.get(target) ?? new Set<SymbolListener>();
    set.add(listener);
    this.watches.set(target, set);

    return () => {
      this.unwatch(target, listener);
    };
  }

  unwatch(target: string, listener?: SymbolListener) {
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

  private dispatchWatch(symbol: MT4Symbol) {
    const listeners = this.watches.get(symbol.symbol);
    if (!listeners) return;

    for (const listener of listeners) {
      listener(symbol);
    }
  }

  private assertLogin(login: number): void {
    if (!Number.isInteger(login) || login <= 0) {
      throw new TypeError("login must be a positive integer");
    }
  }

  private assertRequiredString(value: string, field: string): void {
    if (typeof value !== "string" || value.trim() === "") {
      throw new TypeError(`${field} must be a non-empty string`);
    }
  }

  private normalizeString(value?: string): string | undefined {
    if (value === undefined) {
      return undefined;
    }

    const trimmed = value.trim();
    return trimmed === "" ? undefined : trimmed;
  }
}
