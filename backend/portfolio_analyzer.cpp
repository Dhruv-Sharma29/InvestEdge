// portfolio_analyzer.cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <sstream>
#include <numeric>
#include <iomanip>

using namespace std;

namespace PortfolioAnalyzer {

/*===========================
   Stock Data Structure
===========================*/
struct Stock {
    string symbol, name, sector;
    double price{}, prevClose{}, changePercent{}, marketCap{};
};

/*===========================
   Global Containers
===========================*/
vector<Stock> stocks;
unordered_map<string, int> stockIndex;
unordered_map<string, vector<string>> sectorGraph;

/*===========================
   Load CSV File
===========================*/
bool loadCSV(const string &file) {
    ifstream in(file);
    if (!in.is_open()) {
        cout << "Error: Could not open file " << file << endl;
        return false;
    }

    string line;
    getline(in, line); // Skip header

    while (getline(in, line)) {
        stringstream ss(line);
        string sym, name, sector, price, prev, mc;

        getline(ss, sym, ',');
        getline(ss, name, ',');
        getline(ss, sector, ',');
        getline(ss, price, ',');
        getline(ss, prev, ',');
        getline(ss, mc, ',');

        if (sym.empty()) continue;

        Stock s;
        s.symbol = sym;
        s.name = name;
        s.sector = sector;
        s.price = stod(price);
        s.prevClose = stod(prev);
        s.marketCap = stod(mc);
        s.changePercent = ((s.price - s.prevClose) / s.prevClose) * 100.0;

        stockIndex[s.symbol] = stocks.size();
        stocks.push_back(s);
    }
    return true;
}

/*===========================
   Priority Queue Comparators
===========================*/
struct MaxCmp {
    bool operator()(int a, int b) const {
        return stocks[a].changePercent < stocks[b].changePercent;
    }
};

struct MinCmp {
    bool operator()(int a, int b) const {
        return stocks[a].changePercent > stocks[b].changePercent;
    }
};

/*===========================
   Display Top Gainers & Losers
===========================*/
void showTopMovers(int k) {
    priority_queue<int, vector<int>, MaxCmp> gainers;
    priority_queue<int, vector<int>, MinCmp> losers;

    for (int i = 0; i < (int)stocks.size(); i++) {
        gainers.push(i);
        losers.push(i);
    }

    cout << "\nTop " << k << " Gainers:\n";
    cout << "----------------------------------------\n";
    for (int i = 0; i < k && !gainers.empty(); i++) {
        auto &s = stocks[gainers.top()];
        gainers.pop();
        cout << left << setw(8) << s.symbol << " | "
             << setw(20) << s.name << " | "
             << fixed << setprecision(2) << s.changePercent << "%\n";
    }

    cout << "\nTop " << k << " Losers:\n";
    cout << "----------------------------------------\n";
    for (int i = 0; i < k && !losers.empty(); i++) {
        auto &s = stocks[losers.top()];
        losers.pop();
        cout << left << setw(8) << s.symbol << " | "
             << setw(20) << s.name << " | "
             << fixed << setprecision(2) << s.changePercent << "%\n";
    }
}

/*===========================
   Sort & Display Rankings
===========================*/
void showRankings() {
    vector<int> idx(stocks.size());
    iota(idx.begin(), idx.end(), 0);

    cout << "\nRank by Percentage Change:\n";
    sort(idx.begin(), idx.end(), [](int a, int b) {
        return stocks[a].changePercent > stocks[b].changePercent;
    });

    for (int i = 0; i < min((int)idx.size(), 10); i++) {
        auto &s = stocks[idx[i]];
        cout << i + 1 << ". " << s.symbol
             << " (" << fixed << setprecision(2) << s.changePercent << "%)\n";
    }

    cout << "\nRank by Market Cap:\n";
    sort(idx.begin(), idx.end(), [](int a, int b) {
        return stocks[a].marketCap > stocks[b].marketCap;
    });

    for (int i = 0; i < min((int)idx.size(), 10); i++) {
        auto &s = stocks[idx[i]];
        cout << i + 1 << ". " << s.symbol
             << " (₹" << fixed << setprecision(2) << s.marketCap << ")\n";
    }
}

/*===========================
   Build Sector Graph
===========================*/
void buildSectorGraph() {
    unordered_set<string> sectors;
    for (auto &s : stocks) sectors.insert(s.sector);

    vector<string> sectorList(sectors.begin(), sectors.end());

    for (auto &a : sectorList)
        for (auto &b : sectorList)
            if (a != b) sectorGraph[a].push_back(b);
}

/*===========================
   Show Sector Relationships
===========================*/
void printSectorGraph() {
    cout << "\nSector Dependency Graph\n------------------------\n";
    for (auto &p : sectorGraph) {
        cout << p.first << " -> ";
        for (auto &nb : p.second) cout << nb << " ";
        cout << "\n";
    }
}

/*===========================
   Search Stock
===========================*/
void lookupStock() {
    string sym;
    cout << "Enter stock symbol: ";
    cin >> sym;

    if (!stockIndex.count(sym)) {
        cout << "Stock not found!\n";
        return;
    }

    auto &s = stocks[stockIndex[sym]];
    cout << "\nStock Details\n------------\n";
    cout << "Symbol : " << s.symbol << "\n";
    cout << "Name   : " << s.name << "\n";
    cout << "Sector : " << s.sector << "\n";
    cout << "Price  : " << s.price << "\n";
    cout << "Change : " << s.changePercent << "%\n";
}

/*===========================
           Menu
===========================*/
void menu() {
    while (true) {
        cout << "\n=========== Portfolio Analyzer ===========\n";
        cout << "1. Search Stock\n";
        cout << "2. Top Gainers / Losers\n";
        cout << "3. Show Rankings\n";
        cout << "4. Show Sector Graph\n";
        cout << "0. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter choice: ";

        int ch;
        cin >> ch;

        if (ch == 0) break;
        if (ch == 1) lookupStock();
        else if (ch == 2) {
            int k;
            cout << "How many stocks? ";
            cin >> k;
            showTopMovers(k);
        } else if (ch == 3) showRankings();
        else if (ch == 4) printSectorGraph();
        else cout << "Invalid choice! Try again.\n";
    }
}

/*===========================
         RUN WRAPPER
===========================*/
void run() {
    string file = "portfolio.csv";

    if (!loadCSV(file)) {
        cout << "Please create portfolio.csv first!\n";
        return;
    }

    buildSectorGraph();
    cout << "Loaded " << stocks.size() << " stocks from portfolio.csv\n";

    menu();
}

} // namespace PortfolioAnalyzer
