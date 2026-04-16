import test from "node:test";
import assert from "node:assert/strict";
import path from "node:path";

import MT4Manager from "../src";

test("login fails before connect", async () => {
  const dllPath = path.resolve("dll", "mtmanapi64.dll");
  const manager = new MT4Manager(dllPath);

  await assert.rejects(
    () => manager.login(123456, "secret"),
    /Cannot login before connect/,
  );

  await manager.close();
});
