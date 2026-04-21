import { EventEmitter } from "node:events";
import { NativeFeature } from "../native/types";
import { Listener } from "../types/misc";
import { CreateUserInput, UpdateUserInput, User } from "../types/user";

type UserListener = Listener<User>;

export class UsersService extends EventEmitter {
  constructor(
    private readonly native: NativeFeature<
      User,
      CreateUserInput,
      UpdateUserInput
    >,
  ) {
    super();
    this.native._setUpdateHandler((user) => {
      this.emit("update", user);
      this.dispatchWatch(user);
    });
  }

  private watches = new Map<number, Set<UserListener>>();

  get(login: number) {
    this.assertLogin(login);
    return this.native.get(login);
  }

  create(input: CreateUserInput) {
    this.assertLogin(input.login);
    this.assertRequiredString(input.group, "group");

    return this.native.create({
      ...input,
      name: this.normalizeString(input.name),
      email: this.normalizeString(input.email),
      password: this.normalizeString(input.password),
      investorPassword: this.normalizeString(input.investorPassword),
    });
  }

  update(input: UpdateUserInput) {
    this.assertLogin(input.login);

    if (input.group !== undefined) {
      this.assertRequiredString(input.group, "group");
    }

    return this.native.update({
      ...input,
      group: this.normalizeString(input.group),
      name: this.normalizeString(input.name),
      email: this.normalizeString(input.email),
    });
  }

  delete(login: number) {
    this.assertLogin(login);
    return this.native.delete(login);
  }

  watch(target: number, listener: UserListener) {
    const set = this.watches.get(target) ?? new Set<UserListener>();
    set.add(listener);
    this.watches.set(target, set);

    return () => {
      this.unwatch(target, listener);
    };
  }

  unwatch(target: number, listener?: UserListener) {
    if (!listener) {
      this.watches.delete(target);
      return;
    }

    const set = this.watches.get(target);
    if (!set) {
      return;
    }

    set.delete(listener);

    if (set.size === 0) {
      this.watches.delete(target);
    }
  }

  unwatchAll() {
    this.watches.clear();
  }

  private dispatchWatch(user: User) {
    const listeners = this.watches.get(user.login);
    if (!listeners) return;

    for (const listener of listeners) {
      listener(user);
    }
  }

  // public changePassword(input: ChangeUserPasswordInput) {
  //   this.assertLogin(input.login);
  //   this.assertRequiredString(input.password, "password");

  //   return this.native.changePassword({
  //     ...input,
  //     password: input.password.trim(),
  //     investor: input.investor ?? false,
  //   });
  // }

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
