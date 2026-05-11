import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

test("symbols.get returns normalized symbol info", async () => {
  const pump = await createMT4Manager({
    dllPath: config.dllPath,
    server: config.server,
    login: config.login,
    password: config.password,
    pump: {
      ticks: true,
    },
  });

  await pump.symbols.subscribe("EURUSD.");

  try {
    const done = new Promise<void>((resolve, reject) => {
      const timeout = setTimeout(() => {
        reject(new Error("Timed out waiting for pumped user update"));
      }, 30000);

      pump.symbols.watch("EURUSD.", () => {
        clearTimeout(timeout);

        resolve();
      });
    });

    await done;

    const symbol = await pump.symbols.get("EURUSD.");

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
    await pump.close();
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
