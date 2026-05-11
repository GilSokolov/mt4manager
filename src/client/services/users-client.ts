import { WorkerClient } from "../worker-client";

import {
  CreateUserInput,
  MarginLevel,
  UpdateUserInput,
  User,
} from "../../types/user";

import { EventTypeName } from "../../types";
import { Client } from "../client";

export class UsersClient extends Client<User, EventTypeName, "login"> {
  constructor(worker: WorkerClient) {
    super("login", "users", worker);
  }

  async get(login: number): Promise<User | null> {
    this.assertLogin(login);

    try {
      return await this.worker.call("users.get", {
        login,
      });
    } catch (err: any) {
      if (err?.code === "NOT_FOUND") {
        return null;
      }

      throw err;
    }
  }

  async create(input: CreateUserInput): Promise<User> {
    return this.worker.call("users.create", {
      input,
    });
  }

  async update(login: number, input: UpdateUserInput): Promise<User> {
    this.assertLogin(login);

    return this.worker.call("users.update", {
      login,
      input,
    });
  }

  // ---------------------------------------------------------
  // MARGIN
  // ---------------------------------------------------------

  async getMarginLevel(login: number): Promise<MarginLevel> {
    this.assertLogin(login);

    return this.worker.call("users.getMarginLevel", {
      login,
    });
  }

  // ---------------------------------------------------------
  // VALIDATION
  // ---------------------------------------------------------

  private assertLogin(login: number): void {
    if (!Number.isInteger(login) || login <= 0) {
      throw new TypeError("login must be a positive integer");
    }
  }
}
