import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

test("createMT4Manager starts pumping when pump is true", async () => {
  const pump = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
    pump: true,
  });

  const manager = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
  });

  try {
    let received: any = null;

    const done = new Promise<void>((resolve, reject) => {
      const timeout = setTimeout(() => {
        reject(new Error("Timed out waiting for pumped user update"));
      }, 30000);

      pump.users.once("update", (user) => {
        clearTimeout(timeout);
        received = user;
        resolve();
      });
    });

    await manager.users.update(config.userLogin, {
      name: Math.random().toFixed(5),
    });

    await done;

    assert.ok(received);
    assert.equal(typeof received.login, "number");
  } finally {
    await manager.close();
    await pump.close();
  }
});
