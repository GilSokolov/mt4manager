import test from "node:test";
import assert from "node:assert/strict";

import { MT4Manager } from "../src/manager";
import { config } from "./config";

async function setup() {
  const manager = new MT4Manager(config.dllPath);
  await manager.connect(config.server);
  await manager.login(config.login, config.password);
  return manager;
}

test("users.get returns null when not found", async () => {
  const manager = await setup();

  const nonExistingLogin = 999999999; // pick something safe

  const user = await manager.users.get(nonExistingLogin);

  assert.equal(user, null);

  await manager.close();
});
