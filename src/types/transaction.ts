/**
 * 
type = TT_BR_BALANCE

cmd = OP_BALANCE or OP_CREDIT

orderby = the login of the client, for whom the transaction is performed

expiration = credit expiration date (for cmd = OP_CREDIT)

pirce = the amount of the operation (a positive value to deposit and a negative to withdraw money)

comment = a comment to an operation
 */

import { TradeCommand, TradeReason, TradeRecord } from "./trade-record";

export interface TransactionRequest {
  login: number;
  amount: number;
  comment?: string;
  expiration?: Date;
}

export class Transaction {
  id: number;
  login: number;
  cmd: TradeCommand | number;
  time: Date | null;
  expiration: Date | null;
  reason: TradeReason | number;
  comment: string;
  timestamp: Date | null;

  constructor(trade: TradeRecord) {
    this.id = trade.id;
    this.login = trade.login;
    this.cmd = trade.cmd;
    this.time = trade.openTime;
    this.expiration = trade.expiration;
    this.reason = trade.reason;
    this.comment = trade.comment;
    this.timestamp = trade.timestamp;
  }
}
