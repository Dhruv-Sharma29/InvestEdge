import test from 'node:test';
import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { once } from 'node:events';
import { io } from 'socket.io-client';
import { fileURLToPath } from 'node:url';

test('the live socket rejects traversal and malformed start payloads without crashing', async () => {
  const child = spawn(process.execPath, [fileURLToPath(new URL('../server.js', import.meta.url))], {
    env: { ...process.env, PORT: '0' }, stdio: ['ignore', 'pipe', 'pipe'],
  });
  let socket;
  try {
    const url = await new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(new Error('Server startup timed out')), 5000);
      child.once('error', reject);
      child.stdout.on('data', chunk => {
        const match = chunk.toString().match(/http:\/\/localhost:(\d+)/);
        if (match) { clearTimeout(timer); resolve(`http://127.0.0.1:${match[1]}`); }
      });
    });
    socket = io(url, { transports: ['websocket'], reconnection: false, timeout: 3000 });
    await once(socket, 'connect');
    for (const payload of [null, {}, { appName: '../../../../bin/sh' }, { appName: '/bin/sh' }, { appName: {} }]) {
      const rejected = once(socket, 'errorMsg');
      socket.emit('start', payload);
      const [message] = await rejected;
      assert.equal(message, 'Unknown program.');
    }
    const [response] = await Promise.all([
      once(socket, 'errorMsg'),
      Promise.resolve(socket.emit('sendInput', 'echo should-never-run')),
    ]);
    assert.equal(response[0], 'No running process.');
    assert.equal(child.exitCode, null);
    socket.close();
    const hostile = io(url, { transports: ['websocket'], reconnection: false, timeout: 1000,
      extraHeaders: { Origin: 'https://untrusted.example' } });
    await once(hostile, 'connect_error');
    assert.equal(hostile.connected, false);
    hostile.close();
  } finally {
    socket?.close();
    if (child.exitCode === null && child.signalCode === null) {
      child.kill();
      await once(child, 'exit');
    }
  }
});
