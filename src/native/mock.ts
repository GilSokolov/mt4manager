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
    };
  }

  public async create(input: CreateUserInput): Promise<User> {
    return {
      login: input.login,
      group: input.group,
      name: input.name,
      email: input.email,
      leverage: input.leverage,
    };
  }

  public async update(input: UpdateUserInput): Promise<User> {
    return {
      login: input.login,
      group: input.group ?? "demo\\group",
      name: input.name,
      email: input.email,
      leverage: input.leverage,
    };
  }

  public async delete(_login: number): Promise<void> {}

  public async changePassword(_input: ChangeUserPasswordInput): Promise<void> {}
}

export class MockNativeMT4Manager implements NativeMT4Manager {
  public readonly users = new MockNativeUsersApi();

  public async connect(server: string): Promise<void> {}

  public async login(_login: number, _password: string): Promise<void> {}

  public async disconnect(): Promise<void> {}

  public close(): void {}
}
