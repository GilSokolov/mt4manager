import { EventType } from "./events-types";
import { NativeSymbolsApi } from "./symbol";
import { TradeRecord } from "./trade-record";
import { TradeRequest } from "./trade-request";
import { CreateUserInput, MarginLevel, UpdateUserInput, User } from "./user";

export type Listener<T> = (data: T) => void;

export type PumpingOptions = {
  ticks: boolean;
  news: boolean;
  mail: boolean;
  onlineUsers: boolean;
  users: boolean;
  fullNews: boolean;
};

export enum MT4LogLevel {
  Error = 1,
  Info = 2,
  Debug = 3,
}

export type ManagerConfig = {
  dllPath: string;
  logLevel?: MT4LogLevel;
};

export interface NativeTradeApi {
  get(id: number): Promise<TradeRecord>;
  onEvent(handler: (user: TradeRecord, type: EventType) => void): void;
  handleBidAskUpdate(symbol: string): void;
  execute(request: TradeRequest): Promise<TradeRecord | boolean>;
}

export interface NativeUsersApi {
  get(login: number): Promise<User>;
  getMarginLevel(login: number): MarginLevel;
  create(input: CreateUserInput): Promise<User>;
  update(login: number, input: UpdateUserInput): Promise<User>;
  handleTradeUpdate(login: number): void;
  onEvent(handler: (user: User, type: EventType) => void): void;
}

export interface NativeMT4Manager {
  connect(server: string): Promise<void>;
  login(login: number, password: string): Promise<void>;
  disconnect(): Promise<void>;
  close(): Promise<void>;
  startPumping(options: PumpingOptions): Promise<void>;
  stopPumping(): Promise<void>;

  users: NativeUsersApi;
  symbols: NativeSymbolsApi;
  trades: NativeTradeApi;
}

export type MT4ManagerFactoryConfig = {
  dllPath: string;
  server: string;
  login: number;
  password: string;
  pump?: Partial<PumpingOptions> | true;
  logLevel?: MT4LogLevel;
};
