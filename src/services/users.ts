import { NativeMT4Manager } from "../native/types";
import {
  ChangeUserPasswordInput,
  CreateUserInput,
  UpdateUserInput,
} from "../types/user";

export class UsersService {
  public constructor(private readonly native: NativeMT4Manager) {}

  public get(login: number) {
    this.assertLogin(login);
    return this.native.users.get(login);
  }

  public create(input: CreateUserInput) {
    this.assertLogin(input.login);
    this.assertRequiredString(input.group, "group");

    return this.native.users.create({
      ...input,
      name: this.normalizeString(input.name),
      email: this.normalizeString(input.email),
      password: this.normalizeString(input.password),
      investorPassword: this.normalizeString(input.investorPassword),
    });
  }

  public update(input: UpdateUserInput) {
    this.assertLogin(input.login);

    if (input.group !== undefined) {
      this.assertRequiredString(input.group, "group");
    }

    return this.native.users.update({
      ...input,
      group: this.normalizeString(input.group),
      name: this.normalizeString(input.name),
      email: this.normalizeString(input.email),
    });
  }

  public delete(login: number) {
    this.assertLogin(login);
    return this.native.users.delete(login);
  }

  public changePassword(input: ChangeUserPasswordInput) {
    this.assertLogin(input.login);
    this.assertRequiredString(input.password, "password");

    return this.native.users.changePassword({
      ...input,
      password: input.password.trim(),
      investor: input.investor ?? false,
    });
  }

  private assertLogin(login: number): void {
    if (!Number.isInteger(login) || login <= 0) {
      throw new TypeError("login must be a positive integer");
    }
  }

  private assertRequiredString(value: string, field: string): void {
    if (typeof value !== "string" || value.trim() === "") {
      throw new TypeError(`${field} must be a non-empty string`);
    }
  }

  private normalizeString(value?: string): string | undefined {
    if (value === undefined) {
      return undefined;
    }

    const trimmed = value.trim();
    return trimmed === "" ? undefined : trimmed;
  }
}
