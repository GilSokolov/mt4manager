import test from "node:test";
import assert from "node:assert/strict";

import MT4Manager from "../src";
import { config } from "./config";

test("users emits pumped updates to js", async () => {
  const manager = new MT4Manager(config.dllPath);

  await manager.connect(config.server);
  await manager.login(config.login, config.password);

  let received: any = null;

  const done = new Promise<void>((resolve, reject) => {
    const timeout = setTimeout(() => {
      reject(new Error("Timed out waiting for pumped user update"));
    }, 30000);

    manager.users.once("update", (user) => {
      clearTimeout(timeout);
      received = user;
      resolve();
    });
  });

  await manager.startPumping();

  await done;

  assert.ok(received);
  assert.equal(typeof received.login, "number");

  await manager.close();
});
