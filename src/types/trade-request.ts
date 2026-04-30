import { TradeCommand } from "./trade-record";

export enum TradeRequestFlags {
  NONE = 0,
  SIGNAL = 1,
  EXPERT = 2,
  GATEWAY = 4,
  MOBILE = 8,
  WEB = 16,
  API = 32,
}

export enum TradeExecutionMode {
  Open = "open",
  Close = "close",
  Delete = "delete", // delete order (ANY OPEN ORDER!!!)
  CloseBy = "close_by", // close order by order
  CloseAll = "close_all",
  Modify = "modify", // modify open price, stoploss, takeprofit etc. of order
  Activate = "activate", // activate pending order
  ModifyComment = "modify_comment",
  Balance = "balance",
}

export interface TradeRequest {
  login: number;
  symbol: string;
  volume: number;

  cmd: TradeCommand; // enum (OP_BUY, OP_SELL, etc.)

  id?: number;
  price?: number;
  sl?: number;
  tp?: number;

  deviation?: number;
  comment?: string;

  expiration?: Date;

  // optional meta
  flags?: TradeRequestFlags;
  mode?: TradeExecutionMode;
}
