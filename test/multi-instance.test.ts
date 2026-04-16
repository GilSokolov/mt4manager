import test from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';

import MT4Manager from '../src';

test('multiple instances can be created independently', async () => {
  const dllPath = path.resolve('dll', 'mtmanapi64.dll');
  const a = new MT4Manager(dllPath);
  const b = new MT4Manager(dllPath);

  assert.ok(a);
  assert.ok(b);
  assert.notEqual(a, b);
});
