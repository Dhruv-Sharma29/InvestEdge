# InvestEdge backend

Five interactive C++ tools are streamed over Socket.IO. This is a local academic prototype without account authentication.

## Build and run

Install Node.js 22.12+, CMake 3.20+, a C++17 compiler, and libcurl development headers. CMake fetches the pinned nlohmann/json dependency; no manually downloaded `json.hpp` is required.

```sh
cd backend
cmake -S . -B build
cmake --build build
npm install
npm test
npm run dev
```

Set `TWELVE_DATA_API_KEY` for price tracking and risk management, and `NEWS_API_KEY` for news. Keys are read from the environment at runtime; never put them into source files. Missing keys stop the module with an explanatory error.

The server binds to `127.0.0.1:5055`. Allowed browser origins default to `http://localhost:5173` and `http://127.0.0.1:5173`; `ALLOWED_ORIGINS` accepts a comma-separated override. Binaries are resolved only from this directory's `build` folder and only for the five allowlisted programs. Remote hosting needs a separate authentication and deployment design.
