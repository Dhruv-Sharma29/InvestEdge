import express from 'express';
import cors from 'cors';
import { Server } from 'socket.io';
import { createServer } from 'http';
import { spawn } from 'child_process';
import { BACKEND_DIR, PROGRAMS, resolveBinary } from './programs.js';

const app = express();
const origins = (process.env.ALLOWED_ORIGINS || 'http://localhost:5173,http://127.0.0.1:5173').split(',').map(s => s.trim()).filter(Boolean);
app.use(cors({ origin: origins }));
app.use(express.json());

const httpServer = createServer(app);
const io = new Server(httpServer, {
  cors: { origin: origins },
  allowRequest: (req, callback) => callback(null, !req.headers.origin || origins.includes(req.headers.origin)),
});

const sessions = new Map();

io.on('connection', (socket) => {
  socket.on('start', (payload) => {
    const appName = payload?.appName;
    if (typeof appName !== 'string' || !PROGRAMS.includes(appName)) { socket.emit('errorMsg', 'Unknown program.'); return; }
    if (sessions.has(socket.id)) { socket.emit('errorMsg', 'A process is already running. Stop it first.'); return; }
    const bin = resolveBinary(appName);
    if (!bin) { socket.emit('errorMsg', `Binary not found for ${appName}. Expected in backend/build`); return; }
    const child = spawn(bin, [], { cwd: BACKEND_DIR, shell: false });

    sessions.set(socket.id, { proc: child, appName });

    socket.emit('status', `Started ${appName}`);
    child.stdout.on('data', d => socket.emit('stdout', d.toString()));
    child.stderr.on('data', d => socket.emit('stderr', d.toString()));
    child.on('close', (code) => { socket.emit('status', `Exited (${code})`); sessions.delete(socket.id); });
    child.on('error', (err) => { socket.emit('errorMsg', err.message); sessions.delete(socket.id); });
  });

  socket.on('sendInput', (text) => {
    if (typeof text !== 'string' || text.length > 65536) { socket.emit('errorMsg', 'Invalid input.'); return; }
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
httpServer.listen(PORT, '127.0.0.1', () => {
  console.log(`InvestEdge Backend on http://localhost:${httpServer.address().port}`);
  console.log('Place the five compiled binaries in backend/build');
});