import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

import { TradeCommand, TradeExecutionMode } from "../src/types";

test("positions.open opens a market position", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  try {
    const trade = await manager.positions.open({
      login: config.userLogin,
      symbol: "EURUSD.",
      cmd: TradeCommand.Buy,
      volume: 1, // 0.01 lot in MT4 units
      comment: "node-test-open-position",
      price: 1.17,
    });

    await manager.positions.close({
      volume: 1, // 0.01 lot in MT4 units
      id: trade.id,
      price: 1.17,
    });

    console.log(trade);
    assert.ok(trade);
    assert.equal(typeof trade, "object");
    assert.equal(typeof trade.id, "number");
    assert.ok(trade.id > 0);
    assert.equal(trade.login, config.userLogin);
    assert.equal(trade.symbol, "EURUSD.");
    assert.equal(trade.cmd, TradeCommand.Buy);
    assert.equal(trade.volume, 1);
    assert.equal(typeof trade.openTime, "object");
    assert.ok(trade.openTime instanceof Date);
    assert.equal(typeof trade.openPrice, "number");
  } catch (e) {
    console.log(e);
  } finally {
    // await manager.close();
  }
});

// test("positions.get returns positions", async () => {
//   const manager = await createMT4Manager(config);

//   const positions = await manager.positions.get(19077920);

//   assert.ok(positions);

//   console.log(positions);

//   await manager.close();
// });
