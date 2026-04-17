import test from "node:test";
import assert from "node:assert/strict";

import { MockNativeMT4Manager } from "../src/native/mock";

test("users.get should return user", async () => {
  const manager = new MockNativeMT4Manager();

  const user = await manager.users.get(123);

  assert.ok(user);
  assert.equal(user.login, 123);
  assert.ok(user.group);
});

test("users.create should create user", async () => {
  const manager = new MockNativeMT4Manager();

  const user = await manager.users.create({
    login: 1000,
    group: "demo\\group",
    name: "John Doe",
    email: "john@test.com",
    leverage: 100,
  });

  assert.equal(user.login, 1000);
  assert.equal(user.group, "demo\\group");
  assert.equal(user.name, "John Doe");
});

test("users.update should update user", async () => {
  const manager = new MockNativeMT4Manager();

  const user = await manager.users.update({
    login: 1000,
    name: "Updated Name",
  });

  assert.equal(user.login, 1000);
  assert.equal(user.name, "Updated Name");
});

test("users.delete should resolve", async () => {
  const manager = new MockNativeMT4Manager();

  await manager.users.delete(1000);

  // if no throw, success
  assert.ok(true);
});

test("users.changePassword should resolve", async () => {
  const manager = new MockNativeMT4Manager();

  await manager.users.changePassword({
    login: 1000,
    password: "new-pass",
  });

  assert.ok(true);
});
