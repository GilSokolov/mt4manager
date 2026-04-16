import test from "node:test";
import assert from "node:assert/strict";
import { config } from "./config";
import MT4Manager from "../src";

test("login fails before connect", async () => {
  const manager = new MT4Manager(config.dllPath);

  await assert.rejects(
    () => manager.login(config.login, config.password),
    /Cannot login before connect/,
  );

  await manager.close();
});
