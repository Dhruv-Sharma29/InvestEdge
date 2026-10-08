import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { PROGRAMS, resolveBinary } from '../programs.js';

test('only the five exact program names can resolve', () => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'investedge-'));
  try {
    for (const name of PROGRAMS) {
      fs.writeFileSync(path.join(dir, name), '');
      assert.equal(resolveBinary(name, dir), fs.realpathSync(path.join(dir, name)));
    }
    for (const name of ['../../../../bin/sh', '/bin/sh', '../profit_loss', 'profit_loss.exe', '', '__proto__', null, {}, 1]) {
      assert.equal(resolveBinary(name, dir), null);
    }
    fs.unlinkSync(path.join(dir, 'profit_loss'));
    fs.symlinkSync('/bin/sh', path.join(dir, 'profit_loss'));
    assert.equal(resolveBinary('profit_loss', dir), null);
    assert.equal(resolveBinary('stock_news', path.join(dir, 'missing')), null);
  } finally { fs.rmSync(dir, { recursive: true, force: true }); }
});
