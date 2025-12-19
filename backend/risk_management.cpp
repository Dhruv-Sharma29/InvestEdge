// g++ real_time_tracker_with_risk.cpp -o build/real_time_tracker_with_risk -std=c++17 -lcurl -pthread
#include <iostream>
#include <iomanip>
#include <string>
#include <deque>
#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include "json.hpp"   // https://github.com/nlohmann/json

using json = nlohmann::json;
using namespace std;

namespace RealTimeTrackerWithRisk {

/* ---------------------- Real-Time Tracker ---------------------- */
class RealTimePriceTracker {
private:
    deque<double> window;
    multiset<double> bst;
    size_t window_size;
    double sum;
    mutex mtx;

public:
    RealTimePriceTracker(size_t n = 10) : window_size(n), sum(0.0) {}

    void addPrice(double price) {
        lock_guard<mutex> lock(mtx);
        window.push_back(price);
        bst.insert(price);
        sum += price;
        if (window.size() > window_size) {
            double old = window.front();
            window.pop_front();
            bst.erase(bst.find(old));
            sum -= old;
        }
    }

    double getAverage() const {
        if (window.empty()) return NAN;
        return sum / window.size();
    }

    double getMin() const { return bst.empty() ? NAN : *bst.begin(); }
    double getMax() const { return bst.empty() ? NAN : *bst.rbegin(); }

    void printStats() const {
        cout << fixed << setprecision(2);
        cout << "Min: $" << getMin()
             << " | Max: $" << getMax()
             << " | Avg: $" << getAverage() << "\n";
    }
};

/* ---------------------- CURL JSON Fetching ---------------------- */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

double getStockPrice(const string& symbol, const string& apiKey) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    string url = "https://api.twelvedata.com/price?symbol=" + symbol + "&apikey=" + apiKey;
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    try {
        auto j = json::parse(readBuffer);
        if (j.contains("price")) return stod(j["price"].get<string>());
    } catch (exception& e) {
        cerr << "JSON Parse Error: " << e.what() << endl;
    }
    return -1.0;
}

/* ---------------------- ENTRY POINT ---------------------- */
void run() {
    string apiKey = "YOUR_API_KEY";   // your Twelve Data API key
    string symbol;
    double stopLoss = 0.0, target = 0.0;

    RealTimePriceTracker tracker(10);

    cout << "📈 Real-Time Price Tracker with Risk Management\n";
    cout << "Enter Stock Symbol (type 'exit' to quit): ";
    cin >> symbol;
    if (symbol == "exit" || symbol == "EXIT") return;

    cout << "Enter Stop-Loss threshold: ";
    cin >> stopLoss;
    cout << "Enter Target threshold: ";
    cin >> target;

    cout << "\nMonitoring " << symbol
         << " | Stop-Loss: $" << stopLoss
         << " | Target: $" << target << "\n";

    while (true) {
        double price = getStockPrice(symbol, apiKey);
        if (price <= 0) {
            cerr << "⚠️  Failed to fetch price for " << symbol << ". Retrying...\n";
        } else {
            cout << "\nCurrent price of " << symbol << ": $" << price << endl;
            tracker.addPrice(price);
            tracker.printStats();

            if (price <= stopLoss)
                cout << "🚨 [ALERT] Stop-Loss triggered! Price fell to $" << price << "\n";
            else if (price >= target)
                cout << "🎯 [ALERT] Target reached! Price rose to $" << price << "\n";
        }

        cout << "⏳ Waiting 30 seconds for next update...\n";
        cout << "---------------------------------------------\n";
        this_thread::sleep_for(chrono::seconds(30));
    }
}

} // namespace RealTimeTrackerWithRisk
