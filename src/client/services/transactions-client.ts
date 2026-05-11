import { WorkerClient } from "../worker-client";

import { User } from "../../types/user";

import { EventTypeName, TradeCommand, TradeExecutionMode } from "../../types";
import { Transaction, TransactionRequest } from "../../types/transaction";
import { isTransactionTrade } from "../../utils/is-transaction";
import {
  ExpirationMode,
  resolveExpiration,
} from "../../utils/resolve-expiration";
import { Client } from "../client";

function getExpirationMode(cmd: TradeCommand, sign: 1 | -1): ExpirationMode {
  if (cmd === TradeCommand.Credit && sign === 1) {
    return "required"; // credit in
  }

  return "none"; // balance + credit out
}

export class TransactionsClient extends Client<User, EventTypeName, "login"> {
  constructor(worker: WorkerClient) {
    super("login", "users", worker);
  }

  async get(id: number): Promise<Transaction | null> {
    if (typeof id !== "number") {
      throw new TypeError("id must be number");
    }

    try {
      const trade = await this.worker.call("trades.get", {
        id,
      });

      if (!isTransactionTrade(trade)) {
        return null;
      }

      return new Transaction(trade);
    } catch (err: any) {
      if (err?.code === "NOT_FOUND") {
        return null;
      }

      throw err;
    }
  }

  private async executeTransaction(
    input: TransactionRequest,
    cmd: TradeCommand.Balance | TradeCommand.Credit,
    sign: 1 | -1,
  ) {
    if (!Number.isFinite(input.amount)) {
      throw new TypeError("amount must be a finite number");
    }

    if (input.amount === 0) {
      throw new RangeError("amount must not be zero");
    }

    const mode = getExpirationMode(cmd, sign);

    const request = {
      mode: TradeExecutionMode.Balance,
      cmd,
      login: input.login,
      price: Math.abs(input.amount) * sign,
      comment: input.comment || "",
      expiration: resolveExpiration(mode, input.expiration),
    };

    const transaction = await this.worker.call("trades.execute", {
      request,
    });

    return new Transaction(transaction);
  }

  deposit(input: TransactionRequest) {
    return this.executeTransaction(input, TradeCommand.Balance, 1);
  }

  withdrawal(input: TransactionRequest) {
    return this.executeTransaction(input, TradeCommand.Balance, -1);
  }

  creditIn(input: TransactionRequest) {
    return this.executeTransaction(input, TradeCommand.Credit, 1);
  }

  creditOut(input: TransactionRequest) {
    return this.executeTransaction(input, TradeCommand.Credit, -1);
  }
}
