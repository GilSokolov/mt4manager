import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

test("symbols.get returns normalized symbol info", async () => {
  const manager = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
    pump: {
      ticks: true,
    },
    logLevel: 3,
  });

  try {
    const done = new Promise<void>((resolve) => {
      setTimeout(() => {
        resolve();
      }, 6000);
    });

    await done;

    const symbol = await manager.symbols.get("EURUSD.");

    assert.equal(symbol.symbol, "EURUSD.");
    assert.equal(typeof symbol.digits, "number");
    assert.equal(typeof symbol.visible, "boolean");
    assert.equal(
      symbol.lastTime === null || symbol.lastTime instanceof Date,
      true,
    );
    assert.equal(typeof symbol.bid, "number");
    assert.equal(typeof symbol.ask, "number");
  } finally {
    await manager.close();
  }
});

test("symbols.getAll returns symbol configs", async () => {
  const manager = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
    pump: {
      ticks: true,
    },
    logLevel: 3,
  });

  try {
    const symbols = await manager.symbols.getAll();

    assert.ok(Array.isArray(symbols));
    assert.ok(symbols.length > 0);

    assert.equal(typeof symbols[0].symbol, "string");
    assert.equal(typeof symbols[0].digits, "number");
    assert.equal(typeof symbols[0].point, "number");
    assert.equal(typeof symbols[0].spread, "number");
  } finally {
    await manager.close();
  }
});
