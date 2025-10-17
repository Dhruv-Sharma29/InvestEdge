import express from 'express';
import cors from 'cors';
import { Server } from 'socket.io';
import { createServer } from 'http';
import { spawn } from 'child_process';
import path from 'path';
import fs from 'fs';

const app = express();
app.use(cors());
app.use(express.json());

const httpServer = createServer(app);
const io = new Server(httpServer, { cors: { origin: '*' } });

function resolveBinary(name) {
  const candidates = [
    path.join(process.cwd(), 'build', name),
    path.join(process.cwd(), '..', 'build', name),
    path.join(process.cwd(), name),
  ];
  for (const c of candidates) {
    if (fs.existsSync(c)) return c;
    if (fs.existsSync(c + '.exe')) return c + '.exe';
  }
  return null;
}

const sessions = new Map();

io.on('connection', (socket) => {
  socket.on('start', ({ appName }) => {
    if (sessions.has(socket.id)) { socket.emit('errorMsg', 'A process is already running. Stop it first.'); return; }
    const bin = resolveBinary(appName);
    if (!bin) { socket.emit('errorMsg', `Binary not found for ${appName}. Expected in ./build or ../build`); return; }
    const child = spawn(bin, [], { cwd: process.cwd() });

    sessions.set(socket.id, { proc: child, appName });

    socket.emit('status', `Started ${appName}`);
    child.stdout.on('data', d => socket.emit('stdout', d.toString()));
    child.stderr.on('data', d => socket.emit('stderr', d.toString()));
    child.on('close', (code) => { socket.emit('status', `Exited (${code})`); sessions.delete(socket.id); });
    child.on('error', (err) => { socket.emit('errorMsg', err.message); sessions.delete(socket.id); });
  });

  socket.on('sendInput', (text) => {
    const s = sessions.get(socket.id);
    if (!s?.proc) { socket.emit('errorMsg', 'No running process.'); return; }
    try { s.proc.stdin.write(text + '\n'); } catch (e) { socket.emit('errorMsg', e.message); }
  });

  socket.on('stop', () => {
    const s = sessions.get(socket.id);
    if (s?.proc) { try { s.proc.kill('SIGKILL'); } catch {} sessions.delete(socket.id); socket.emit('status', 'Stopped'); }
  });

  socket.on('disconnect', () => {
    const s = sessions.get(socket.id);
    if (s?.proc) { try { s.proc.kill('SIGKILL'); } catch {} sessions.delete(socket.id); }
  });
});

app.get('/health', (_req, res) => res.json({ ok: true }));

const PORT = process.env.PORT || 5055;
httpServer.listen(PORT, () => {
  console.log(`InvestEdge Backend on http://localhost:${PORT}`);
  console.log('Place compiled binaries in ./build (profit_loss, real_time_tracker, stock_news)');
});