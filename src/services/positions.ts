import { EventEmitter } from "node:events";
import { Listener } from "../types";
import { TradeCommand, TradeRecord, EventTypeName } from "../types";
import { TradeExecutionMode, TradeRequest } from "../types/trade-request";
import { resolveExpiration } from "../utils/resolve-expiration";

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
  }

  handleEvent = (event: EventTypeName, trade: TradeRecord) => {
    if (!isPositionTrade(trade)) {
      return;
    }

    this.emit(event, trade);
    this.dispatchWatch(trade);
  };

  on(eventName: EventTypeName, listener: (trade: TradeRecord) => void): this {
    return super.on(eventName, listener);
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

  cancel(id: number) {
    return this.native.execute({
      mode: TradeExecutionMode.Delete,
      id,
    });
  }

  closeBy(id: number, targetId: number) {
    return this.native.execute({
      mode: TradeExecutionMode.CloseBy,
      id,
      login: targetId,
    });
  }

  closeAll(login: number, symbol: string) {
    return this.native.execute({
      mode: TradeExecutionMode.CloseAll,
      symbol,
      login,
    });
  }

  modify(
    input: Pick<TradeRequest, "id" | "tp" | "sl" | "price" | "expiration">,
  ) {
    return this.native.execute({
      mode: TradeExecutionMode.Modify,
      id: input.id,
      tp: input.tp,
      sl: input.sl,
      price: input.price,
      expiration: resolveExpiration("optional", input.expiration),
    });
  }

  modifyCommnet(id: string, comment: string) {
    return this.native.execute({
      mode: TradeExecutionMode.ModifyComment,
      id,
      comment,
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
    expiration: resolveExpiration("optional", input.expiration),
    deviation: input.deviation ?? 0,
    comment: input.comment ?? "",
    flags: input.flags ?? 0,
    mode,
  };
}
