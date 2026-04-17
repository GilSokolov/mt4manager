import test from "node:test";
import assert from "node:assert/strict";

import MT4Manager from "../src";
import { config } from "./config";

function createManager() {
  return new MT4Manager({ dllPath: config.dllPath });
}

test("constructor: throws if dllPath missing", () => {
  assert.throws(() => new MT4Manager({} as any));
});

test("constructor: throws if dllPath not string", () => {
  assert.throws(() => new MT4Manager({ dllPath: 123 as any }));
});

test("constructor: throws if dllPath invalid", () => {
  assert.throws(() => new MT4Manager({ dllPath: "C:\\bad\\path.dll" }));
});

test("login before connect should fail", async () => {
  const manager = createManager();

  try {
    await assert.rejects(
      () => manager.login(config.login, config.password),
      /cannot login before connect/i,
    );
  } finally {
    manager.close();
  }
});

test("connect after close should fail", async () => {
  const manager = createManager();

  try {
    manager.close();
    await assert.rejects(() => manager.connect(config.server));
  } finally {
    manager.close();
  }
});

test("login after disconnect should fail", async () => {
  const manager = createManager();

  try {
    await manager.connect(config.server);
    await manager.disconnect();

    await assert.rejects(
      () => manager.login(config.login, config.password),
      /cannot login before connect|not connected/i,
    );
  } finally {
    manager.close();
  }
});

test("close twice is safe", () => {
  const manager = createManager();

  manager.close();
  assert.doesNotThrow(() => manager.close());
});
