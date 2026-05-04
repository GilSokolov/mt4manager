import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager, TradeCommand } from "../src";
import { config } from "./config";

test("trades emits pumped updates to js", async () => {
  try {
    const manager = await createMT4Manager({ ...config });

    const pump = await createMT4Manager({ ...config });

    let received: any = null;

    const done = new Promise<void>((resolve, reject) => {
      const timeout = setTimeout(() => {
        reject(new Error("Timed out waiting for pumped trade update"));
      }, 30000);

      pump.positions.once("update", (trade) => {
        clearTimeout(timeout);
        received = trade;
        resolve();
      });
    });

    await pump.startPumping();

    const trade = await manager.positions.open({
      login: config.userLogin,
      symbol: "EURUSD.",
      cmd: TradeCommand.Buy,
      volume: 1, // 0.01 lot in MT4 units
      comment: "node-test-open-position",
      price: 1.17,
    });

    await done;

    console.log(received);

    assert.ok(received);
    assert.equal(typeof received.login, "number");

    await manager.close();
    await pump.close();
  } catch (error) {
    console.log(error);
  }
});
