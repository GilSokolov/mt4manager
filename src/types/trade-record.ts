export enum TradeCommand {
  Buy = 0,
  Sell = 1,
  BuyLimit = 2,
  SellLimit = 3,
  BuyStop = 4,
  SellStop = 5,
  Balance = 6,
  Credit = 7,
}

export enum TradeState {
  OpenNormal = 0,
  OpenRemand = 1,
  OpenRestored = 2,
  ClosedNormal = 3,
  ClosedPart = 4,
  ClosedBy = 5,
  Deleted = 6,
}

export enum TradeReason {
  Client = 0,
  Expert = 1,
  Dealer = 2,
  Signal = 3,
  Gateway = 4,
  Mobile = 5,
  Web = 6,
  Api = 7,
}

export enum TradeActivation {
  StopoutRollback = -4,
  PendingRollback = -3,
  TpRollback = -2,
  SlRollback = -1,

  None = 0,

  Sl = 1,
  Tp = 2,
  Pending = 3,
  Stopout = 4,
}

export interface TradeRecord {
  id: number;
  login: number;
  symbol: string;
  digits: number;
  cmd: TradeCommand | number;
  volume: number;

  openTime: Date | null;
  openPrice: number;
  sl: number;
  tp: number;
  closeTime: Date | null;
  closePrice: number;
  expiration: Date | null;
  reason: TradeReason | number;

  convRates: [number, number];

  commission: number;
  commissionAgent: number;
  swap: number;
  profit: number;
  taxes: number;
  magic: number;
  comment: string;
  gwOrder: number;
  gwVolume: number;
  gwOpenPrice: number;
  gwClosePrice: number;
  marginRate: number;
  timestamp: Date | null;
  activation: TradeActivation | number;
  state: TradeState | number;
  apiData: [number, number, number, number];
}
