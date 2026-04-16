import test from "node:test";
import assert from "node:assert/strict";
import path from "node:path";
import MT4Manager from "../src";

const dllPath =
  process.env.MT4_DLL_PATH || path.resolve("dll", "mtmanapi64.dll");
const server = process.env.MT4_SERVER;

test("connect integration", async (t) => {
  if (!server) {
    t.skip("MT4_SERVER not set");
    return;
  }

  const manager = new MT4Manager(dllPath);

  await manager.connect(server);
  await manager.close();

  assert.ok(true);
});
