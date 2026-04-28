import assert from "node:assert/strict";
import test from "node:test";

import { MT4Manager } from "../src/manager";
import { config } from "./config";

function makeManager() {
  return new MT4Manager(config.dllPath);
}

async function openManager() {
  const manager = makeManager();
  await manager.connect(config.server);
  await manager.login(config.login, config.password);
  return manager;
}

function uniqueEmail(prefix = "user") {
  return `${prefix}_${Date.now()}_${Math.floor(Math.random() * 100000)}@example.com`;
}

function uniqueName(prefix = "Test User") {
  return `${prefix} ${Date.now()} ${Math.floor(Math.random() * 100000)}`;
}

test("users.get should return user", async () => {
  const manager = new MT4Manager(config.dllPath);

  await manager.connect(config.server);
  await manager.login(config.login, config.password);

  const user = await manager.users.get(config.userLogin);

  if (user) {
    assert.equal(user.login, config.userLogin);
    assert.ok(typeof user.group === "string");
  }

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

test("users.create validates arguments", async (t) => {
  const manager = await openManager();

  t.after(() => {
    manager.close();
  });

  assert.rejects(
    () => (manager.users.create as any)(),
    /Expected object for argument: user/,
  );

  assert.rejects(
    () => (manager.users.create as any)("bad"),
    /Expected object for argument: user/,
  );
});

test("users.update validates arguments", async (t) => {
  const manager = await openManager();

  t.after(() => {
    manager.close();
  });

  assert.rejects(
    () => (manager.users.update as any)(),
    /Expected number for argument: login/,
  );

  assert.rejects(
    () => (manager.users.update as any)({ name: "bad" }, { name: "test" }),
    /Expected number for argument: login/,
  );

  assert.rejects(
    () => (manager.users.update as any)(config.userLogin),
    /Expected object for argument: data/,
  );

  assert.rejects(
    () => (manager.users.update as any)(config.userLogin, "bad"),
    /Expected object for argument: data/,
  );
});

test("users.create creates a new user and returns login", async (t) => {
  const manager = await openManager();

  t.after(() => {
    manager.close();
  });

  const email = uniqueEmail("create");
  const name = uniqueName("Create");
  const group = config.testGroup;

  if (!group) {
    throw new Error("MT4_TEST_GROUP is required for tests");
  }

  const user = await manager.users.create({
    group,
    name,
    email,
    password: "Secret123!",
    passwordInvestor: "Investor123!",
    leverage: 100,
  });

  assert.ok(user);

  assert.equal(user.name, name);
  assert.equal(user.email, email);
  assert.equal(user.group, group);
  assert.equal(user.leverage, 100);
});

test("users.update updates an existing user", async (t) => {
  const manager = await openManager();

  t.after(() => {
    manager.close();
  });

  const group = config.testGroup;

  if (!group) {
    throw new Error("MT4_TEST_GROUP is required for tests");
  }

  const res = await manager.users.create({
    group,
    name: uniqueName("Before Update"),
    email: uniqueEmail("before_update"),
    password: "Secret123!",
    passwordInvestor: "Investor123!",
    leverage: 100,
  });

  assert.equal(typeof res.login, "number");
  assert.ok(res.login > 0);

  const updatedName = uniqueName("After Update");
  const updatedEmail = uniqueEmail("after_update");

  const user = await manager.users.update(res.login, {
    name: updatedName,
    email: updatedEmail,
    leverage: 200,
  });

  assert.equal(user.login, res.login);
  assert.equal(user.name, updatedName);
  assert.equal(user.email, updatedEmail);
  assert.equal(user.group, group);
  assert.equal(user.leverage, 200);
});
