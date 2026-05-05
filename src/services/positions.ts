import { Listener } from "../types";
import { TradeCommand, TradeRecord } from "../types/trade-record";
import { TradeExecutionMode, TradeRequest } from "../types/trade-request";
import { EventEmitter } from "node:events";

type TradeRecordListener = Listener<TradeRecord>;

function isPositionTrade(trade: TradeRecord) {
  return (
    trade.cmd !== TradeCommand.Balance && trade.cmd !== TradeCommand.Credit
  );
}

export class Positions extends EventEmitter {
  private watches = new Map<number, Set<TradeRecordListener>>();

  constructor(private native: any) {
    super();
    this.native._setUpdateHandler((trade: TradeRecord) => {
      if (isPositionTrade(trade)) {
        this.emit("update", trade);
        this.dispatchWatch(trade);
      }
    });
  }

  HandleBidAskUpdate(symbol: string) {
    return this.native.handleBidAskUpdate(symbol);
  }

  get(order: number): Promise<TradeRecord> {
    if (typeof order !== "number") {
      return Promise.reject("order must be number");
    }

    return this.native.get(order);
  }

  open(input: TradeRequest) {
    const request = normalizeOpenTrade(input, TradeExecutionMode.Open);

    return this.native.execute(request);
  }

  close(input: Pick<TradeRequest, "id" | "deviation" | "volume" | "price">) {
    return this.native.execute({
      mode: TradeExecutionMode.Close,
      id: input.id,
      volume: input.volume,
      deviation: input.deviation || 0,
      price: input.price,
    });
  }

  private dispatchWatch(trade: TradeRecord) {
    const listeners = this.watches.get(trade.login);
    if (!listeners) return;

    for (const listener of listeners) {
      listener(trade);
    }
  }

  watch(target: number, listener: TradeRecordListener) {
    const set = this.watches.get(target) ?? new Set<TradeRecordListener>();
    set.add(listener);
    this.watches.set(target, set);

    return () => {
      this.unwatch(target, listener);
    };
  }

  unwatch(target: number, listener?: TradeRecordListener) {
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
}

function normalizeOpenTrade(input: TradeRequest, mode: TradeExecutionMode) {
  if (!input || typeof input !== "object") {
    throw new Error("Expected object for trade");
  }

  if (typeof input.login !== "number") {
    throw new Error("login is required");
  }

  if (!input.symbol) {
    throw new Error("symbol is required");
  }

  return {
    ...input,
    expiration: input.expiration
      ? Math.floor(input.expiration.getTime() / 1000)
      : 0,
    deviation: input.deviation ?? 0,
    comment: input.comment ?? "",
    flags: input.flags ?? 0,
    mode,
  };
}
