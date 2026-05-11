import { parentPort, workerData } from "node:worker_threads";
import { EventMessage } from "../client/worker-client";
import { Transaction } from "../types/transaction";
import { isTransactionTrade } from "../utils/is-transaction";
import { loadBinding } from "../utils/paths";
import { toEventType } from "../utils/to-event-type";
import { dispatchRpc } from "./rpc-dispatcher";

const { MT4Manager } = loadBinding();

const manager = new MT4Manager(workerData.config);

manager.users.onEvent((user, type) => {
  try {
    const event = toEventType(type);

    if (event === "unknown") return;

    parentPort!.postMessage({
      data: user,
      event,
      kind: "event",
      scope: "users",
    } as EventMessage);
  } catch (error) {
    console.log(error);
  }
});

manager.symbols.onEvent((data) => {
  try {
    parentPort!.postMessage({
      data,
      event: "tick",
      kind: "event",
      scope: "symbols",
    } as EventMessage);

    manager.trades.handleBidAskUpdate(data.symbol);
  } catch (error) {
    console.log(error);
  }
});

manager.trades.onEvent((trade, type) => {
  try {
    const event = toEventType(type);

    if (event === "unknown") return;

    if (isTransactionTrade(trade)) {
      parentPort!.postMessage({
        data: new Transaction(trade),
        event,
        kind: "event",
        scope: "transactions",
      } as EventMessage);
    } else {
      parentPort!.postMessage({
        data: trade,
        event,
        kind: "event",
        scope: "positions",
      } as EventMessage);
    }

    if (event === "add" || event === "delete") {
      manager.users.handleTradeUpdate(trade.login);
    }

    if (event === "update") {
      parentPort!.postMessage({
        data: manager.users.getMarginLevel(trade.login),
        event,
        kind: "event",
        scope: "margin",
      } as EventMessage);
    }
  } catch (error) {
    console.log(error);
  }
});

parentPort!.on("message", async (msg) => {
  try {
    const result = await dispatchRpc(manager, msg.method, msg.params);

    parentPort!.postMessage({
      id: msg.id,
      result,
    });
  } catch (err: any) {
    parentPort!.postMessage({
      id: msg.id,
      error: {
        name: err.name,
        message: err.message,
        code: err.code,
        stack: err.stack,
      },
    });
  }
});
