import assert from "node:assert/strict";
import test from "node:test";

import { MT4Manager } from "../src/manager";
import { config } from "./config";

test("users.get should return user", async () => {
  const manager = new MT4Manager(config.dllPath);

  await manager.connect(config.server);
  await manager.login(config.login, config.password);

  const user = await manager.users.get(config.userLogin);

  assert.equal(user.login, config.userLogin);
  assert.ok(typeof user.group === "string");

  manager.close();
});

test("users.get should reject invalid login", () => {
  const manager = new MT4Manager(config.dllPath);

  assert.throws(() => manager.users.get(0), {
    name: "TypeError",
    message: "login must be a positive integer",
  });

  manager.close();
});

test("users subscribe/unsubscribe does not throw", async () => {
  const manager = new MT4Manager(config.dllPath);

  manager.users.subscribe(123);
  manager.users.unsubscribe(123);
  manager.users.unsubscribeAll();

  await manager.close();
});
