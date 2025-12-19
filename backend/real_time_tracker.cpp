#include <iostream>
#include <iomanip>
#include <string>
#include <deque>
#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include "json.hpp" // Download from: https://github.com/nlohmann/json

using json = nlohmann::json;
using namespace std;

namespace RealTimeTracker {

    // ---------------------- Real-Time Tracker Class ----------------------
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

        void printStats() {
            lock_guard<mutex> lock(mtx);
            cout << fixed << setprecision(2);

            double min = bst.empty() ? NAN : *bst.begin();
            double max = bst.empty() ? NAN : *bst.rbegin();
            double avg = window.empty() ? NAN : sum / window.size();

            cout << "Min: $" << min
                 << " | Max: $" << max
                 << " | Avg: $" << avg << "\n";
        }
    };

    // ---------------------- CURL JSON Fetching ----------------------
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
            if (j.contains("price")) {
                return stod(j["price"].get<string>());
            } else {
                cerr << "API Error: " << j.dump() << endl;
            }
        } catch (exception& e) {
            cerr << "JSON Parse Error: " << e.what() << endl;
        }
        return -1.0;
    }

    // ---------------------- MODULE ENTRY POINT ----------------------
    void run() {
        string apiKey = "YOUR_API_KEY"; // 🔑 Replace with your Twelve Data API key
        string symbol;

        RealTimePriceTracker tracker(10);

        cout << " Real-Time Price Tracker Started!\n";
        cout << "Enter stock symbols one by one (type 'exit' to quit).\n\n";

        while (true) {
            cout << "Enter Stock Symbol: ";
            cin >> symbol;

            if (symbol == "exit" || symbol == "EXIT") {
                cout << "👋 Exiting Real-Time Tracker. Goodbye!\n";
                break;
            }

            double price = getStockPrice(symbol, apiKey);

            if (price > 0) {
                cout << "\nCurrent price of " << symbol << ": $" << price << endl;
                tracker.addPrice(price);
                tracker.printStats();
            } else {
                cerr << "Failed to fetch price for " << symbol << ".\n";
            }

            cout << "\n Waiting 5 seconds before next input...\n";
            this_thread::sleep_for(chrono::seconds(5));
            cout << "-----------------------------\n";
        }
    }

} // namespace RealTimeTracker
