import assert from "node:assert/strict";
import test from "node:test";

import { MT4Manager } from "../src/manager";
import { config } from "./config";

test("users.get should return user", async () => {
  const manager = new MT4Manager(config.dllPath);

  const user = await manager.users.get(123);

  assert.equal(user.login, 123);
  assert.equal(user.group, "demo\\group");
  assert.equal(user.name, "Test User");
  assert.equal(user.email, "test@example.com");
  assert.equal(user.leverage, 100);
});

test("users.get should reject invalid login", () => {
  const manager = new MT4Manager(config.dllPath);

  assert.throws(() => manager.users.get(0), {
    name: "TypeError",
    message: "login must be a positive integer",
  });

  manager.close();
});
