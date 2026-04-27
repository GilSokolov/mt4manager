import test from "node:test";
import assert from "node:assert/strict";

import MT4Manager from "../src";
import { config } from "./config";

test("users emits pumped updates to js", async () => {
  const pump = new MT4Manager({ dllPath: config.dllPath });
  const manager = new MT4Manager({ dllPath: config.dllPath });

  await pump.connect(config.server);
  await pump.login(config.login, config.password);

  await manager.connect(config.server);
  await manager.login(config.login, config.password);

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

  await pump.startPumping();

  await manager.users.update(config.userLogin, {
    name: Math.random().toFixed(5),
  });

  await done;

  assert.ok(received);
  assert.equal(typeof received.login, "number");

  await manager.close();
  await pump.close();
});
