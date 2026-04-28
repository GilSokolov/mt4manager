export interface MT4Symbol {
  symbol: string;
  digits: number;
  visible: boolean;
  type: number;
  point: number;
  spread: number;
  direction: number;
  lastTime: Date | null;
  bid: number;
  ask: number;
  high: number;
  low: number;
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

export interface NativeSymbolsApi {
  get(symbol: string): Promise<MT4Symbol>;
  getAll(): Promise<MT4SymbolConfig[]>;
  subscribe(symbol: string): Promise<void>;
  unsubscribe(symbol: string): Promise<void>;

  _setUpdateHandler(handler: (user: MT4Symbol) => void): void;
}
