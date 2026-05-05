import { EventEmitter } from "node:events";
import { Listener } from "../types";
import { TradeCommand, TradeRecord } from "../types/trade-record";
import { TradeExecutionMode } from "../types/trade-request";
import { Transaction, TransactionRequest } from "../types/transaction";
import { ExpirationMode, resolveExpiration } from "../utils/resolveExpiration";

type TradeRecordListener = Listener<TradeRecord>;

function isTransactionTrade(trade: TradeRecord) {
  return (
    trade.cmd === TradeCommand.Balance || trade.cmd === TradeCommand.Credit
  );
}

function getExpirationMode(cmd: TradeCommand, sign: 1 | -1): ExpirationMode {
  if (cmd === TradeCommand.Credit && sign === 1) {
    return "required"; // credit in
  }

  return "none"; // balance + credit out
}

export class Transactions extends EventEmitter {
  private watches = new Map<number, Set<TradeRecordListener>>();

  constructor(private native: any) {
    super();
    this.native._setUpdateHandler((trade: TradeRecord) => {
      if (isTransactionTrade(trade)) {
        this.emit("update", trade);
        this.dispatchWatch(trade);
      }
    });
  }

  HandleBidAskUpdate(symbol: string) {
    return this.native.handleBidAskUpdate(symbol);
  }

  async get(id: number): Promise<Transaction | null> {
    if (typeof id !== "number") {
      throw new TypeError("id must be number");
    }

    try {
      const trade = await this.native.get(id);

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

    const res = await this.native.execute({
      mode: TradeExecutionMode.Balance,
      cmd,
      login: input.login,
      price: Math.abs(input.amount) * sign,
      comment: input.comment || "",
      expiration: resolveExpiration(mode, input.expiration),
    });

    return new Transaction(res);
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
