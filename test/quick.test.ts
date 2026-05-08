import assert from "node:assert/strict";
import test from "node:test";

import { config } from "./config";
import { createMT4Manager } from "../src";

test("users.get should return user", async () => {
  const manager = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
    logLevel: 3,
  });

  try {
    const user = await manager.users.get(config.userLogin);

    if (user) {
      assert.equal(user.login, config.userLogin);
      assert.ok(typeof user.group === "string");
    }
  } catch (error) {
    console.log(error);
  } finally {
    manager.close();
  }
});
