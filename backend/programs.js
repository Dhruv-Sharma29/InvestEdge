import path from 'node:path';
import fs from 'node:fs';
import { fileURLToPath } from 'node:url';

export const PROGRAMS = Object.freeze([
  'profit_loss', 'real_time_tracker', 'stock_news',
  'risk_management', 'portfolio_analyzer',
]);
export const BACKEND_DIR = path.dirname(fileURLToPath(import.meta.url));

export function resolveBinary(name, buildDir = path.join(BACKEND_DIR, 'build')) {
  if (typeof name !== 'string' || !PROGRAMS.includes(name)) return null;
  if (!fs.existsSync(buildDir)) return null;
  const root = fs.realpathSync(buildDir);
  for (const suffix of ['', '.exe']) {
    const candidate = path.join(root, name + suffix);
    if (!fs.existsSync(candidate)) continue;
    const real = fs.realpathSync(candidate);
    if (path.dirname(real) === root && fs.statSync(real).isFile()) return real;
  }
  return null;
}
