// src/App.jsx
import React, { useEffect, useRef, useState } from 'react'
import './ui/investedge.css'
import logoUrl from './ui/logo.svg'
import { io } from 'socket.io-client'

const SERVER_URL = 'http://localhost:5055'

export default function InvestEdge() {
  const [socket, setSocket] = useState(null)
  const [connected, setConnected] = useState(false)
  const [status, setStatus] = useState('Idle')
  const [pick, setPick] = useState('profit_loss')
  const [log, setLog] = useState('')
  const inputRef = useRef(null)

  useEffect(() => {
    const s = io(SERVER_URL, { transports: ['websocket'] })
    setSocket(s)
    s.on('connect', () => setConnected(true))
    s.on('disconnect', () => setConnected(false))
    s.on('stdout', chunk => setLog(p => p + chunk))
    s.on('stderr', chunk => setLog(p => p + chunk))
    s.on('status', msg => { setStatus(msg); setLog(p => p + `\n[STATUS] ${msg}\n`) })
    s.on('errorMsg', msg => setLog(p => p + `\n[ERROR] ${msg}\n`) )
    return () => s.disconnect()
  }, [])

  const start = () => socket?.emit('start', { appName: pick })
  const stop = () => socket?.emit('stop')
  const send = () => {
    const v = inputRef.current?.value ?? ''
    if (!v.trim()) return
    socket?.emit('sendInput', v)
    inputRef.current.value = ''
  }

  return (
    <div>
      {/* ------------------ Navbar ------------------ */}
      <nav className="nav">
        <div className="nav-inner">
          <a className="brand" href="#">
            <img src={logoUrl} className="glow" width="120" height="30" alt="InvestEdge" />
            <span>InvestEdge</span>
          </a>
          <div style={{ marginLeft: 'auto' }} className="badge">
            <span
              style={{
                width: 8,
                height: 8,
                borderRadius: 999,
                background: connected ? '#22c55e' : '#ef4444',
                display: 'inline-block'
              }}
            />
            {connected ? 'Backend connected' : 'Backend offline'}
          </div>
        </div>
      </nav>

      {/* ------------------ Main Section ------------------ */}
      <div className="container">
        <section className="hero">
          <div>
            <div className="badge">Real-time Intelligence</div>
            <h1 className="h1">
              Gain the <em>Edge</em> in Your Investments
            </h1>
            <p className="sub">
              Track performance, stream live prices, analyze portfolios, and scan market news — all from one unified
              dashboard. InvestEdge brings analytics, AI, and automation together for smarter trading.
            </p>

            {/* --- Buttons and Module Selector --- */}
            <div className="cta-row">
              <button className="btn-primary" onClick={start}>
                Start Selected
              </button>
              <button className="btn-ghost" onClick={stop}>
                Stop
              </button>

              <select
                value={pick}
                onChange={e => setPick(e.target.value)}
                className="btn-ghost"
                style={{ cursor: 'pointer' }}
              >
                <option value="profit_loss">Profit / Loss</option>
                <option value="real_time_tracker">Real-Time Tracker</option>
                <option value="stock_news">Stock News</option>
                <option value="risk_management">Risk Management</option>
                <option value="portfolio_analyzer">Portfolio Analyzer</option>
              </select>
            </div>

            {/* --- Input box --- */}
            <div style={{ display: 'grid', gridTemplateColumns: '1fr auto', gap: 12, marginTop: 12 }}>
              <input
                ref={inputRef}
                placeholder="Type input for the running app (Enter sends)"
                onKeyDown={e => {
                  if (e.key === 'Enter') send()
                }}
                style={{
                  padding: '10px 12px',
                  borderRadius: 10,
                  background: '#0f172a',
                  color: '#e5e7eb',
                  border: '1px solid #1f2a49'
                }}
              />
              <button className="btn-primary" onClick={send}>
                Send
              </button>
            </div>

            {/* --- Status Cards --- */}
            <div className="stat-grid">
              <div className="card">
                <h4>Session</h4>
                <div className="metric attn">{status}</div>
              </div>
              <div className="card">
                <h4>Theme</h4>
                <div className="metric">Blue / Green / Black</div>
              </div>
              <div className="card">
                <h4>Latency</h4>
                <div className="metric up">Realtime</div>
              </div>
            </div>
          </div>

          {/* --- Live Console --- */}
          <div className="panel">
            <div className="title">Live Console</div>
            <textarea
              readOnly
              value={log}
              spellCheck={false}
              style={{
                width: '100%',
                height: 260,
                background: 'transparent',
                color: '#d1d5db',
                border: 'none',
                outline: 'none',
                fontFamily:
                  'ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace',
                fontSize: 13
              }}
            />
          </div>
        </section>

        {/* ------------------ Info Panels ------------------ */}
        <section style={{ marginTop: 24 }} className="grid-2">
          <div className="panel">
            <div className="title">Why InvestEdge?</div>
            <ul style={{ margin: 0, paddingLeft: 18, color: '#cbd5e1', lineHeight: 1.7 }}>
              <li>Unified interface for P/L, tracking, risk analysis, and portfolio tools</li>
              <li>Low-latency streaming over WebSockets</li>
              <li>Dark, modern palette that reduces eye strain</li>
              <li>Responsive layout for desktops and laptops</li>
            </ul>
          </div>
          <div className="panel">
            <div className="title">Quick Tips</div>
            <ol style={{ margin: 0, paddingLeft: 18, color: '#cbd5e1', lineHeight: 1.7 }}>
              <li>Compile all binaries and place them in <code>backend/build/</code>.</li>
              <li>Start backend <code>npm run dev</code>, then frontend <code>npm run dev</code>.</li>
              <li>Select a module (e.g. <strong>Risk Management</strong>) and click <strong>Start</strong>.</li>
              <li>Use the console to send inputs or view live outputs.</li>
            </ol>
          </div>
        </section>

        {/* ------------------ Footer ------------------ */}
        <footer className="footer">
          <span>© {new Date().getFullYear()} InvestEdge</span>
          <span style={{ opacity: 0.8 }}>Smart Insights. Sharper Decisions.</span>
        </footer>
      </div>
    </div>
  )
}
