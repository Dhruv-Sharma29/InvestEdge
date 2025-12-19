# InvestEdge – Backend

The backend of **InvestEdge** is responsible for high-performance financial
analytics and API orchestration. It combines **C++ analytics modules** for
computationally intensive tasks with a **Node.js server** that exposes these
capabilities to the frontend.

> ⚠️ **Note:** This is an academic project. Certain configurations (such as API
keys) must be provided by the user before execution.
---

## 🧠 Responsibilities

- Execute high-performance financial computations using C++
- Handle profit & loss calculations
- Perform portfolio analytics and risk assessment
- Track real-time stock data
- Process stock-related news
- Expose results via a Node.js API layer

---

## 🛠️ Tech Stack

- **C++** – Core analytics and computation engines
- **Node.js** – Backend server and API orchestration
- **JSON** – Communication between Node.js and C++ binaries
- **CSV** – Portfolio data input

---

## 🔐 API Key Configuration (Required)

This backend uses **external APIs** to fetch real-time stock prices and market news.
For security reasons, **API keys are not included** in the repository.

### 1️⃣ Real-Time Stock Price API
- **Service:** Twelve Data API  
- **Used in:** `real_time_tracker.cpp`

**Steps:**
1. Create an account at: https://twelvedata.com/
2. Obtain your API key
3. Open:
   ```text
   real_time_tracker.cpp
   ```

4. Replace:
    
    ```cpp
    string apiKey = "YOUR_API_KEY";
    ```
    
    with your own API key **before compiling/running**.
    

---

### 2️⃣ Stock Market News API

- **Service:** NewsAPI
    
- **Used in:** `stock_news.cpp`
    

**Steps:**

1. Create an account at: [https://newsapi.org/](https://newsapi.org/)
    
2. Obtain your API key
    
3. Open:
    
    ```text
    stock_news.cpp
    ```
    
4. Replace:
    
    ```cpp
    string apiKey = "YOUR_API_KEY";
    ```
    
    with your own NewsAPI key **before compiling/running**.

---

## 📁 Directory Overview

```text
backend/
├── profit_loss.cpp              # Profit & Loss computation module
├── real_time_tracker.cpp        # Real-time stock tracking logic
├── portfolio_analyzer.cpp       # Portfolio analytics engine
├── risk_management.cpp          # Risk metrics and analysis
├── stock_news.cpp               # Stock news processing
├── server.js                    # Node.js backend server
├── package.json
├── README.md
├── portfolio.csv                # Sample portfolio data
└── mains/
    ├── profit_loss_main.cpp
    ├── real_time_tracker_main.cpp
    ├── portfolio_analyzer_main.cpp
    ├── risk_management_main.cpp
    └── stock_news_main.cpp
``` 
---

## ⚙️ Setup & Execution

### Prerequisites

- Node.js (v16 or higher)
    
- C++ compiler (GCC / Clang)
    
- npm

---

### Install Dependencies

```bash
npm install
```

---

### Build C++ Binaries

Compile C++ modules and place the generated binaries inside:

```text
./build/
```

(Each `*_main.cpp` file serves as an entry point for its respective module.)

---

### Run Backend Server

```bash
npm run dev
```

The backend server will start and be ready to serve API requests from the frontend.
