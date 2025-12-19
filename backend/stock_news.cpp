#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"  // Include nlohmann/json header

using json = nlohmann::json;
using namespace std;

namespace StockNews {

    // ======== WRITE CALLBACK ========
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }

    // ======== FETCH STOCK NEWS ========
    void FetchStockNews() {
        CURL* curl = curl_easy_init();
        if (!curl) {
            cerr << " CURL initialization failed\n";
            return;
        }

        string apiKey = "YOUR_API_KEY"; 
        string url = "https://newsapi.org/v2/everything?q=finance%20OR%20stocks%20OR%20business&sortBy=publishedAt&pageSize=15&language=en&apiKey=" + apiKey;
        string readBuffer;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "User-Agent: StockNewsApp/1.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            cerr << "Failed to fetch news: " << curl_easy_strerror(res) << endl;
            return;
        }

        try {
            json data = json::parse(readBuffer);
            if (data["status"] != "ok") {
                cout << "API Error: " << data["message"] << endl;
                return;
            }

            cout << "\n===== 📰 Top 15 Stock Market Headlines =====\n";
            int count = 0;

            for (auto& article : data["articles"]) {
                count++;
                cout << "\n-------------------------------------------\n";
                cout << count << ". " << article["title"].get<string>() << endl;
                cout << "Source: " << article["source"]["name"].get<string>() << endl;

                if (!article["author"].is_null())
                    cout << "Author: " << article["author"].get<string>() << endl;
                if (!article["publishedAt"].is_null())
                    cout << "Date: " << article["publishedAt"].get<string>() << endl;
                if (!article["description"].is_null())
                    cout << "\nSummary: " << article["description"].get<string>() << endl;
                if (!article["url"].is_null())
                    cout << "\nRead more: " << article["url"].get<string>() << endl;
            }

            if (count == 0)
                cout << "No news found.\n";

        } catch (exception& e) {
            cerr << " JSON Parsing Error: " << e.what() << endl;
        }
    }

    // ======== MODULE ENTRY POINT ========
    void run() {
        cout << "===== STOCK MARKET NEWS FETCHER =====\n";
        FetchStockNews();
    }

} // namespace StockNews
