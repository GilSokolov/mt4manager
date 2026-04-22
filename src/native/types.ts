import { PumpingOptions } from "../types/misc";
import type {
  ChangeUserPasswordInput,
  CreateUserInput,
  UpdateUserInput,
  User,
} from "../types/user";

export interface NativeUsersApi {
  get(login: number): Promise<User>;
  create(input: CreateUserInput): Promise<number>;
  update(input: UpdateUserInput): Promise<void>;
  delete(login: number): Promise<void>;
  changePassword(input: ChangeUserPasswordInput): Promise<void>;

  _setUpdateHandler: any;
}

export interface NativeFeature<T, C, U> {
  get(login: number): Promise<T>;
  create(input: C): Promise<number>;
  update(input: U): Promise<void>;
  delete(login: number): Promise<void>;

  _setUpdateHandler(handler: (user: T) => void): void;
}

export interface NativeMT4Manager {
  connect(server: string): Promise<void>;
  login(login: number, password: string): Promise<void>;
  disconnect(): Promise<void>;
  close(): void;
  startPumping(options: PumpingOptions): Promise<void>;
  stopPumping(): Promise<void>;

  users: NativeUsersApi;
}
