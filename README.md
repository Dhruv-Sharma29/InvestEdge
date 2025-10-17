# InvestEdge — Full Stack (React + Node)

**Generated:** 2025-10-17T12:42:41.726886Z

## Structure
```
InvestEdge/
  backend/    # Node.js server (Express + socket.io)
  frontend/   # React (Vite) frontend with InvestEdge theme
```

## Quick Start
1. **Place binaries** in `backend/build/` (profit_loss, real_time_tracker, stock_news).
2. **Backend**:
   ```bash
   cd backend
   npm i
   npm run dev
   ```
3. **Frontend** (new terminal):
   ```bash
   cd frontend
   npm i
   npm run dev
   ```
4. Open http://localhost:5173

The frontend connects to the backend on http://localhost:5055 and streams your CLI apps.
