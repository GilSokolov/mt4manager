import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

import { TradeCommand } from "../src/types";

const SYMBOL = "EURUSD.";
const PRICE = 1.17;
const VOLUME = 1;

test("positions.open opens a market position", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  const trade = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Buy,
    volume: VOLUME,
    comment: "node-test-open-position",
    price: PRICE,
  });

  await manager.positions.close({
    volume: VOLUME,
    id: trade.id,
    price: PRICE,
  });

  assert.ok(trade);
  assert.equal(typeof trade, "object");
  assert.equal(typeof trade.id, "number");
  assert.ok(trade.id > 0);
  assert.equal(trade.login, config.userLogin);
  assert.equal(trade.symbol, SYMBOL);
  assert.equal(trade.cmd, TradeCommand.Buy);
  assert.equal(trade.volume, VOLUME);
  assert.ok(trade.openTime instanceof Date);
  assert.equal(typeof trade.openPrice, "number");
});

test("positions.modify modifies an open position", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  const trade = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Buy,
    volume: VOLUME,
    comment: "node-test-modify-position",
    price: PRICE,
  });

  const modified = await manager.positions.modify({
    id: trade.id,
    sl: 1.1,
    tp: 1.2,
    price: PRICE,
  });

  await manager.positions.close({
    id: trade.id,
    volume: VOLUME,
    price: PRICE,
  });

  assert.ok(modified);
});

test("positions.modifyComment modifies position comment", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  const trade = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Buy,
    volume: VOLUME,
    comment: "node-test-before-comment",
    price: PRICE,
  });

  const modified = await manager.positions.modifyComment(
    trade.id,
    "node-test-after-comment",
  );

  await manager.positions.close({
    id: trade.id,
    volume: VOLUME,
    price: PRICE,
  });

  assert.ok(modified);
});

test("positions.cancel cancels a pending position", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  const pending = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.BuyLimit,
    volume: VOLUME,
    comment: "node-test-cancel-position",
    price: 1.0,
  });

  const cancelled = await manager.positions.cancel(pending.id, pending.cmd);

  assert.ok(cancelled);
});

test("positions.closeBy closes opposite positions by ticket", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  const buy = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Buy,
    volume: VOLUME,
    comment: "node-test-closeby-buy",
    price: PRICE,
  });

  const sell = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Sell,
    volume: VOLUME,
    comment: "node-test-closeby-sell",
    price: PRICE,
  });

  const closed = await manager.positions.closeBy(buy.id, sell.id);

  assert.ok(closed);
});

test("positions.closeMultipleBy closes opposite positions", async () => {
  const manager = await createMT4Manager({ ...config, logLevel: 3 });

  const buy = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Buy,
    volume: VOLUME,
    comment: "node-test-close-multiple-by-buy",
    price: PRICE,
  });

  const sell = await manager.positions.open({
    login: config.userLogin,
    symbol: SYMBOL,
    cmd: TradeCommand.Sell,
    volume: VOLUME,
    comment: "node-test-close-multiple-by-sell",
    price: PRICE,
  });

  const result = await manager.positions.closeMultipleBy(
    config.userLogin,
    SYMBOL,
  );

  assert.equal(result, true);
  assert.ok(buy.id > 0);
  assert.ok(sell.id > 0);
});
