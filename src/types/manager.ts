import { NativeUsersApi } from "./user";

export type Listener<T> = (user: T) => void;

export type PumpingOptions = {
  ticks: boolean;
  news: boolean;
  mail: boolean;
  onlineUsers: boolean;
  users: boolean;
  fullNews: boolean;
};

export interface NativeMT4Manager {
  connect(server: string): Promise<void>;
  login(login: number, password: string): Promise<void>;
  disconnect(): Promise<void>;
  close(): void;
  startPumping(options: PumpingOptions): Promise<void>;
  stopPumping(): Promise<void>;

  users: NativeUsersApi;
}
