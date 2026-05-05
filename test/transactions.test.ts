import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

import { TradeCommand } from "../src/types";

test("transactions.get returns a Transaction", async () => {
  const manager = await createMT4Manager({ ...config });

  try {
    const created = await manager.transactions.deposit({
      login: config.userLogin,
      amount: 10,
      comment: "node-test-get-transaction",
    });

    const tx = await manager.transactions.get(created.id);

    assert.ok(tx);
    assert.equal(tx.id, created.id);
    assert.equal(tx.login, config.userLogin);
    assert.equal(tx.cmd, TradeCommand.Balance);

    assert.ok(tx.time === null || tx.time instanceof Date);
    assert.ok(tx.expiration === null || tx.expiration instanceof Date);
    assert.equal(typeof tx.reason, "number");
    assert.equal(typeof tx.comment, "string");
    assert.ok(tx.timestamp === null || tx.timestamp instanceof Date);
  } catch (e) {
    console.log(e);
    throw e;
  }
});

test("transactions.deposit creates a transaction", async () => {
  const manager = await createMT4Manager({ ...config });

  try {
    const tx = await manager.transactions.deposit({
      login: config.userLogin,
      amount: 100,
      comment: "node-test-deposit",
    });

    assert.ok(tx);
    assert.equal(typeof tx, "object");

    assert.equal(typeof tx.id, "number");
    assert.ok(tx.id > 0);

    assert.equal(tx.login, config.userLogin);
    assert.equal(tx.cmd, TradeCommand.Balance);

    assert.ok(tx.time === null || tx.time instanceof Date);
    assert.ok(tx.expiration === null || tx.expiration instanceof Date);
    assert.equal(typeof tx.reason, "number");
    assert.equal(typeof tx.comment, "string");
    assert.ok(tx.timestamp === null || tx.timestamp instanceof Date);
  } catch (e) {
    console.log(e);
    throw e;
  }
});

test("transactions.withdrawal creates a negative balance transaction", async () => {
  const manager = await createMT4Manager({ ...config });

  try {
    const tx = await manager.transactions.withdrawal({
      login: config.userLogin,
      amount: 50,
      comment: "node-test-withdrawal",
    });

    assert.ok(tx);
    assert.equal(typeof tx.id, "number");
    assert.ok(tx.id > 0);

    assert.equal(tx.login, config.userLogin);
    assert.equal(tx.cmd, TradeCommand.Balance);

    assert.ok(tx.time === null || tx.time instanceof Date);
    assert.ok(tx.expiration === null || tx.expiration instanceof Date);
    assert.equal(typeof tx.reason, "number");
    assert.equal(tx.comment, "node-test-withdrawal");
    assert.ok(tx.timestamp === null || tx.timestamp instanceof Date);
  } catch (e) {
    console.log(e);
    throw e;
  }
});

test("transactions.creditIn creates a positive credit transaction", async () => {
  const manager = await createMT4Manager({ ...config });

  try {
    const tx = await manager.transactions.creditIn({
      login: config.userLogin,
      amount: 25,
      comment: "node-test-credit-in",
    });

    assert.ok(tx);
    assert.equal(typeof tx.id, "number");
    assert.ok(tx.id > 0);

    assert.equal(tx.login, config.userLogin);
    assert.equal(tx.cmd, TradeCommand.Credit);

    assert.ok(tx.time === null || tx.time instanceof Date);
    assert.ok(tx.expiration === null || tx.expiration instanceof Date);
    assert.equal(typeof tx.reason, "number");
    assert.equal(tx.comment, "node-test-credit-in");
    assert.ok(tx.timestamp === null || tx.timestamp instanceof Date);
  } catch (e) {
    console.log(e);
    throw e;
  }
});

test("transactions.creditOut creates a negative credit transaction", async () => {
  const manager = await createMT4Manager({ ...config });

  try {
    const tx = await manager.transactions.creditOut({
      login: config.userLogin,
      amount: 25,
      comment: "node-test-credit-out",
    });

    assert.ok(tx);
    assert.equal(typeof tx.id, "number");
    assert.ok(tx.id > 0);

    assert.equal(tx.login, config.userLogin);
    assert.equal(tx.cmd, TradeCommand.Credit);

    assert.ok(tx.time === null || tx.time instanceof Date);
    assert.ok(tx.expiration === null || tx.expiration instanceof Date);
    assert.equal(typeof tx.reason, "number");
    assert.equal(tx.comment, "node-test-credit-out");
    assert.ok(tx.timestamp === null || tx.timestamp instanceof Date);
  } catch (e) {
    console.log(e);
    throw e;
  }
});
