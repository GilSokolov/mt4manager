import type { NativeMT4Manager } from "./types";
import type {
  ChangeUserPasswordInput,
  CreateUserInput,
  UpdateUserInput,
  User,
} from "../types/user";

class MockNativeUsersApi {
  public async get(login: number): Promise<User> {
    return {
      login,
      group: "demo\\group",
      name: "Test User",
      email: "test@example.com",
      leverage: 100,
    } as User;
  }

  public async create(input: CreateUserInput): Promise<User> {
    return {
      login: input.login,
      group: input.group,
      name: input.name,
      email: input.email,
      leverage: input.leverage,
    } as User;
  }

  public async update(input: UpdateUserInput): Promise<User> {
    return {
      login: input.login,
      group: input.group ?? "demo\\group",
      name: input.name,
      email: input.email,
      leverage: input.leverage,
    } as User;
  }

  public async delete(_login: number): Promise<void> {}

  public async changePassword(_input: ChangeUserPasswordInput): Promise<void> {}

  _setUpdateHandler: any;
}

export class MockNativeMT4Manager implements NativeMT4Manager {
  async stopPumping(): Promise<void> {
    throw new Error("Method not implemented.");
  }
  async startPumping(): Promise<void> {
    throw new Error("Method not implemented.");
  }
  public readonly users = new MockNativeUsersApi();

  public async connect(server: string): Promise<void> {}

  public async login(_login: number, _password: string): Promise<void> {}

  public async disconnect(): Promise<void> {}

  public close(): void {}
}
