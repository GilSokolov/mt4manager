import test from "node:test";
import assert from "node:assert/strict";
import MT4Manager from "../src";
import { config } from "./config";

test("connect integration", async (t) => {
  if (!config.server) {
    t.skip("MT4_SERVER not set");
    return;
  }

  const manager = new MT4Manager(config.dllPath);

  await manager.connect(config.server);
  await manager.close();

  assert.ok(true);
});
