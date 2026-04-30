import { TradeRecord } from "../types/trade-record";
import { TradeExecutionMode, TradeRequest } from "../types/trade-request";

export class Positions {
  constructor(private native: any) {}

  get(order: number): Promise<TradeRecord> {
    if (typeof order !== "number") {
      return Promise.reject("order must be number");
    }

    return this.native.get(order);
  }

  open(input: TradeRequest) {
    const request = normalizeOpenTrade(input, TradeExecutionMode.Open);

    return this.native.create(request);
  }

  close(input: Pick<TradeRequest, "id" | "deviation" | "volume" | "price">) {
    return this.native.create({
      mode: TradeExecutionMode.Close,
      id: input.id,
      volume: input.volume,
      deviation: input.deviation || 0,
      price: input.price,
    });

    // mode = TT_BR_ORDER_CLOSE
    // id = the ticket of the order to close
    // price = close price
    // volume = volume in lots to close an order
    // symbol = symbol name (optional)
  }

  //   close(ticket: number) {
  //     if (typeof ticket !== "number") {
  //       return Promise.reject("ticket must be number");
  //     }

  //     return this.native.close(ticket);
  //   }

  //   closeAll(login?: number) {
  //     return this.native.closeAll(login);
  //   }
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
