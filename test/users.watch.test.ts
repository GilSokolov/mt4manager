import test from "node:test";
import assert from "node:assert/strict";

import MT4Manager from "../src";
import { config } from "./config";

test("users.watch(login) filters pumped updates", async () => {
  const manager = new MT4Manager(config.dllPath);

  await manager.connect(config.server);
  await manager.login(config.login, config.password);

  let received: any = null;

  const done = new Promise<void>((resolve, reject) => {
    const timeout = setTimeout(() => {
      reject(new Error("Timed out waiting for watched user update"));
    }, 15000);

    const stop = manager.users.watch(config.userLogin, (user) => {
      clearTimeout(timeout);
      stop();
      received = user;
      resolve();
    });
  });

  await manager.startPumping();

  await done;

  assert.ok(received);
  assert.equal(received.login, config.userLogin);

  await manager.close();
});
