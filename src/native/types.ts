import type {
  ChangeUserPasswordInput,
  CreateUserInput,
  UpdateUserInput,
  User,
} from "../types/user";

export interface NativeUsersApi {
  get(login: number): Promise<User>;
  create(input: CreateUserInput): Promise<User>;
  update(input: UpdateUserInput): Promise<User>;
  delete(login: number): Promise<void>;
  changePassword(input: ChangeUserPasswordInput): Promise<void>;
  subscribe(login: number): void;
  unsubscribe(login: number): void;
  unsubscribeAll(): void;
}

export interface NativeMT4Manager {
  connect(server: string): Promise<void>;
  login(login: number, password: string): Promise<void>;
  disconnect(): Promise<void>;
  close(): void;

  users: NativeUsersApi;
}
