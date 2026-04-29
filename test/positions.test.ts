import test from "node:test";
import assert from "node:assert/strict";

import { createMT4Manager } from "../src";
import { config } from "./config";

test("positions.get returns positions", async () => {
  const manager = await createMT4Manager(config);

  const positions = await manager.positions.get(19077920);

  assert.ok(positions);

  console.log(positions);

  await manager.close();
});
