import { WorkerClient } from "../worker-client";

import { EventTypeName, MT4Symbol, MT4SymbolConfig, Tick } from "../../types";
import { Client } from "../client";

export class SymbolsClient extends Client<Tick, "tick", "symbol"> {
  constructor(worker: WorkerClient) {
    super("symbol", "symbols", worker);
  }

  get(symbol: string): Promise<MT4Symbol> {
    this.assertSymbol(symbol);

    return this.worker.call("symbols.get", {
      symbol,
    });
  }

  getAll(): Promise<MT4SymbolConfig[]> {
    return this.worker.call("symbols.getAll");
  }

  subscribe(symbol: string): Promise<void> {
    this.assertSymbol(symbol);

    return this.worker.call("symbols.subscribe", {
      symbol,
    });
  }

  unsubscribe(symbol: string): Promise<void> {
    this.assertSymbol(symbol);

    return this.worker.call("symbols.unsubscribe", {
      symbol,
    });
  }

  private assertSymbol(symbol: string) {
    if (typeof symbol !== "string") {
      throw new TypeError("Expected string: symbol");
    }

    if (!symbol.trim()) {
      throw new TypeError("Expected non-empty string: symbol");
    }
  }
}
