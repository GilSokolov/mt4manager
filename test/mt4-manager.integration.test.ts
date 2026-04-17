import test from "node:test";
import assert from "node:assert/strict";

import MT4Manager from "../src";
import { config } from "./config";

function createManager() {
  return new MT4Manager({ dllPath: config.dllPath });
}

const slowTest = config.runSlowTests ? test : test.skip;

test("connect integration", async (t) => {
  if (!config.server) {
    t.skip("MT4 server not configured");
    return;
  }

  const manager = createManager();

  try {
    await manager.connect(config.server);
    assert.ok(true);
  } finally {
    manager.close();
  }
});

test("reconnect after disconnect works", async () => {
  const manager = createManager();

  try {
    await manager.connect(config.server);
    await manager.disconnect();
    await manager.connect(config.server);
    assert.ok(true);
  } finally {
    manager.close();
  }
});

test("connect failure includes context", async () => {
  const manager = createManager();

  try {
    await assert.rejects(
      () => manager.connect("bad-config.server"),
      /connect|no connection/i,
    );
  } finally {
    manager.close();
  }
});

test("instances are isolated", async () => {
  const a = createManager();
  const b = createManager();

  try {
    await a.connect(config.server);
    await b.connect(config.server);

    a.close();

    await b.login(config.login, config.password);
    assert.ok(true);
  } finally {
    a.close();
    b.close();
  }
});

test("multiple instances can be created independently", () => {
  const a = createManager();
  const b = createManager();

  try {
    assert.ok(a);
    assert.ok(b);
    assert.notEqual(a, b);
  } finally {
    a.close();
    b.close();
  }
});

slowTest("login failure includes context", async () => {
  const manager = createManager();

  try {
    await manager.connect(config.server);

    await assert.rejects(
      () => manager.login(config.login, "wrong-password"),
      /login|invalid account/i,
    );
  } finally {
    manager.close();
  }
});
