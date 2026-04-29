import { EventEmitter } from "node:events";
import { Listener } from "../types/manager";
import { MT4SymbolConfig, NativeSymbolsApi, Tick } from "../types/symbol";

type SymbolListener = Listener<Tick>;

export class Symbols extends EventEmitter {
  constructor(private readonly native: NativeSymbolsApi) {
    super();
    this.native._setTickHandler((symbol) => {
      this.emit("tick", symbol);
      this.dispatchWatch(symbol);
    });
  }

  private watches = new Map<string, Set<SymbolListener>>();

  get(symbol: string) {
    return this.native.get(symbol);
  }

  getAll(): Promise<MT4SymbolConfig[]> {
    return Promise.resolve(this.native.getAll());
  }

  subscribe(symbol: string): Promise<void> {
    this.assertSymbol(symbol);
    return Promise.resolve(this.native.subscribe(symbol));
  }

  unsubscribe(symbol: string): Promise<void> {
    this.assertSymbol(symbol);
    return Promise.resolve(this.native.unsubscribe(symbol));
  }

  private assertSymbol(symbol: string) {
    if (typeof symbol !== "string") {
      throw new TypeError("Expected string: symbol");
    }

    if (!symbol.trim()) {
      throw new TypeError("Expected non-empty string: symbol");
    }
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

  private dispatchWatch(symbol: Tick) {
    const listeners = this.watches.get(symbol.symbol);
    if (!listeners) return;

    for (const listener of listeners) {
      listener(symbol);
    }
  }
}
