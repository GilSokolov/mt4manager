import { EventEmitter } from "node:events";
import { EventTypeName } from "../types";
import { Listener } from "../types/manager";
import {
  CreateUserInput,
  NativeUsersApi,
  UpdateUserInput,
  User,
} from "../types/user";
import { toEventType } from "../utils/to-event-type";

type UserListener = Listener<User>;

export class Users extends EventEmitter {
  constructor(private readonly native: NativeUsersApi) {
    super();
    this.native._setUpdateHandler((user, type) => {
      const event = toEventType(type);
      if (event !== "unknown") {
        this.emit(event, user);
        this.dispatchWatch(user);
      }
    });
  }

  private watches = new Map<number, Set<UserListener>>();

  on(eventName: EventTypeName, listener: (user: User) => void): this {
    return super.on(eventName, listener);
  }

  async get(login: number): Promise<User | null> {
    try {
      return await this.native.get(login);
    } catch (err: any) {
      if (err?.code === "NOT_FOUND") {
        return null;
      }

      throw err;
    }
  }

  create(input: CreateUserInput) {
    return this.native.create(input);
  }

  update(login: number, input: UpdateUserInput) {
    return this.native.update(login, input);
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
