import { WorkerClient } from "../worker-client";

import { User } from "../../types/user";

import {
  EventTypeName,
  TradeCommand,
  TradeExecutionMode,
  TradeRecord,
  TradeRequest,
} from "../../types";
import { Client } from "../client";
import { resolveExpiration } from "../../utils/resolve-expiration";

export class PositionsClient extends Client<
  TradeRecord,
  EventTypeName,
  "login"
> {
  constructor(worker: WorkerClient) {
    super("login", "positions", worker);
  }

  get(id: number): Promise<TradeRecord> {
    if (typeof id !== "number") {
      return Promise.reject("order must be number");
    }

    return this.worker.call("trades.get", {
      id,
    });
  }

  open(input: TradeRequest): Promise<TradeRecord> {
    if (!input || typeof input !== "object") {
      throw new Error("Expected object for trade");
    }

    if (typeof input.login !== "number") {
      throw new Error("login is required");
    }

    if (!input.symbol) {
      throw new Error("symbol is required");
    }

    const request: TradeRequest = {
      ...input,
      expiration: resolveExpiration("optional", input.expiration),
      deviation: input.deviation ?? 0,
      comment: input.comment ?? "",
      flags: input.flags ?? 0,
      mode: TradeExecutionMode.Open,
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }

  close(
    input: Pick<TradeRequest, "id" | "deviation" | "volume" | "price">,
  ): Promise<TradeRecord> {
    const request = {
      mode: TradeExecutionMode.Close,
      id: input.id,
      volume: input.volume,
      deviation: input.deviation || 0,
      price: input.price,
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }

  cancel(id: number, cmd: TradeCommand): Promise<TradeRecord> {
    const request = {
      mode: TradeExecutionMode.Delete,
      id,
      cmd,
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }

  closeBy(id: number, targetId: number): Promise<TradeRecord> {
    const request = {
      mode: TradeExecutionMode.CloseBy,
      id,
      login: targetId,
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }

  async closeMultipleBy(login: number, symbol: string): Promise<boolean> {
    const request = {
      mode: TradeExecutionMode.CloseAll,
      symbol,
      login,
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }

  modify(
    input: Pick<TradeRequest, "id" | "tp" | "sl" | "price" | "expiration">,
  ): Promise<TradeRecord> {
    const request = {
      mode: TradeExecutionMode.Modify,
      id: input.id,
      tp: input.tp,
      sl: input.sl,
      price: input.price,
      expiration: resolveExpiration("optional", input.expiration),
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }

  modifyComment(id: number, comment: string): Promise<TradeRecord> {
    const request = {
      mode: TradeExecutionMode.ModifyComment,
      id,
      comment,
    };

    return this.worker.call("trades.execute", {
      request,
    });
  }
}
