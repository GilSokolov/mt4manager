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

// test("users.create creates a new user and returns login", async (t) => {
//   const manager = await openManager();

//   t.after(() => {
//     manager.close();
//   });

//   const email = uniqueEmail("create");
//   const name = uniqueName("Create");
//   const group = config.testGroup;

//   if (!group) {
//     throw new Error("MT4_TEST_GROUP is required for tests");
//   }

//   const login = await manager.users.create({
//     group,
//     name,
//     email,
//     password: "Secret123!",
//     passwordInvestor: "Investor123!",
//     leverage: 100,
//   });

//   assert.equal(typeof login, "number");
//   assert.ok(login > 0);

//   const user = await manager.users.get(login);

//   assert.equal(user.login, login);
//   assert.equal(user.name, name);
//   assert.equal(user.email, email);
//   assert.equal(user.group, group);
//   assert.equal(user.leverage, 100);
// });

test("users.update updates an existing user", async (t) => {
  const manager = await openManager();

  t.after(() => {
    manager.close();
  });

  const group = config.testGroup;

  if (!group) {
    throw new Error("MT4_TEST_GROUP is required for tests");
  }

  const login = await manager.users.create({
    group,
    name: uniqueName("Before Update"),
    email: uniqueEmail("before_update"),
    password: "Secret123!",
    passwordInvestor: "Investor123!",
    leverage: 100,
  });

  assert.equal(typeof login, "number");
  assert.ok(login > 0);

  const updatedName = uniqueName("After Update");
  const updatedEmail = uniqueEmail("after_update");

  await manager.users.update({
    login,
    group,
    name: updatedName,
    email: updatedEmail,
    leverage: 200,
    enable: true
  });

  const user = await manager.users.get(login);

  console.log(user);

  assert.equal(user.login, login);
  assert.equal(user.name, updatedName);
  assert.equal(user.email, updatedEmail);
  assert.equal(user.group, group);
  assert.equal(user.leverage, 200);
});
