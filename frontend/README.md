# InvestEdge frontend

React interface for selecting one of five C++ command-line programs, sending text input, and reading their streamed output. The current prototype displays terminal output; it does not implement charts or accounts.

Requires Node.js 22.12 or later. Start the backend on `http://localhost:5055` first, then run:

```sh
npm ci
npm run dev
```

The frontend opens at `http://localhost:5173`. Market-data programs also require provider keys on the backend. See the root README for the C++ build and backend setup.

```sh
npm run build
npm run preview
```

To use the preview server, add its origin (`http://localhost:4173`) to the backend's `ALLOWED_ORIGINS` setting.
