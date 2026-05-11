export interface MT4Symbol {
  symbol: string;
  direction: number;
  bid: number;
  ask: number;
  high: number;
  low: number;
  lastTime: Date | null;
  digits: number;
  visible: boolean;
  type: number;
  point: number;
  spread: number;
  commission: number;
  commissionType: number;
}

export interface MT4SymbolConfig {
  symbol: string;
  description: string;
  currency: string;

  digits: number;
  point: number;

  spread: number;

  contractSize: number;
  tickValue: number;
  tickSize: number;

  stopsLevel: number;

  marginCurrency: string;
  marginInitial: number;
  marginMaintenance: number;
}

export type Tick = Pick<
  MT4Symbol,
  "symbol" | "direction" | "bid" | "ask" | "high" | "low" | "lastTime"
>;

export interface NativeSymbolsApi {
  get(symbol: string): Promise<MT4Symbol>;
  getAll(): Promise<MT4SymbolConfig[]>;
  subscribe(symbol: string): Promise<void>;
  unsubscribe(symbol: string): Promise<void>;
  onEvent(handler: (tick: Tick) => void): void;
}
