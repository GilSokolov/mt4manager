// src/worker/rpc-dispatcher.ts

import { NativeMT4Manager } from "../types/manager";

export async function dispatchRpc(
  manager: NativeMT4Manager,
  method: string,
  params: any,
) {
  switch (method) {
    // -------------------------------------------------
    // USERS
    // -------------------------------------------------

    case "users.get":
      return manager.users.get(params.login);

    case "users.create":
      return manager.users.create(params.input);

    case "users.update":
      return manager.users.update(params.login, params.input);

    case "users.getMarginLevel":
      return manager.users.getMarginLevel(params.login);

    // -------------------------------------------------
    // SYMBOLS
    // -------------------------------------------------

    case "symbols.get":
      return manager.symbols.get(params.login);

    case "symbols.getAll":
      return manager.symbols.getAll();

    case "symbols.subscribe":
      return manager.symbols.subscribe(params.symbol);

    case "symbols.unsubscribe":
      return manager.symbols.unsubscribe(params.symbol);

    // -------------------------------------------------
    // TRADES
    // -------------------------------------------------

    case "trades.get":
      return manager.trades.get(params.id);

    case "trades.execute":
      return manager.trades.execute(params.request);

    // -------------------------------------------------
    // MANAGER
    // -------------------------------------------------

    case "manager.connect":
      return manager.connect(params.server);

    case "manager.login":
      return manager.login(params.login, params.password);

    case "manager.disconnect":
      return manager.disconnect();

    case "manager.close":
      return manager.close();

    // -------------------------------------------------
    // PUMPING
    // -------------------------------------------------

    case "pumping.start":
      return manager.startPumping(params.options);

    case "pumping.stop":
      return manager.stopPumping();

    default:
      throw Object.assign(new Error(`Unknown RPC method: ${method}`), {
        code: "UNKNOWN_RPC_METHOD",
      });
  }
}
