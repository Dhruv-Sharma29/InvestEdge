#include <iostream>
#include <stack>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;


namespace ProfitLossModule{

    // ======== STRUCTURE ========
    struct ProfitLoss {
        string stockname;
        string type;
        int quantity;
        double price;
    };

    // ======== GLOBALS ========
    stack<ProfitLoss> profitlosshistory, redoStack;
    double totalInvestment = 0.0, totalProfitLoss = 0.0;
    const string Fhistory = "History.csv";

    // ======== FILE HANDLING ========
    void savehistory() {
        ofstream file(Fhistory);
        stack<ProfitLoss> temp;
        while (!profitlosshistory.empty()) {
            temp.push(profitlosshistory.top());
            profitlosshistory.pop();
        }
        file << "Stock Name,Type,Quantity,Price\n";
        while (!temp.empty()) {
            ProfitLoss pl = temp.top();
            temp.pop();
            file << pl.stockname << "," << pl.type << "," << pl.quantity << "," << pl.price << "\n";
            profitlosshistory.push(pl);
        }
        file.close();
    }

    void LoadHistory() {
        ifstream file(Fhistory);
        if (!file.is_open()) return;
        string line; getline(file, line); // skip header
        while (getline(file, line)) {
            stringstream ss(line);
            ProfitLoss pl; string qty, pr;
            getline(ss, pl.stockname, ',');
            getline(ss, pl.type, ',');
            getline(ss, qty, ',');
            getline(ss, pr, ',');
            pl.quantity = stoi(qty);
            pl.price = stod(pr);
            profitlosshistory.push(pl);
            if (pl.type == "BUY" || pl.type == "buy")
                totalInvestment += pl.price * pl.quantity;
            else if (pl.type == "SELL" || pl.type == "sell")
                totalProfitLoss += pl.price * pl.quantity;
        }
        file.close();
    }

    // ======== TRADING SYSTEM ========
    void TradeStock() {
        ProfitLoss pl;
        cout << "Enter stock name: ";
        cin >> pl.stockname;
        cout << "Enter type (Buy/Sell): ";
        cin >> pl.type;
        cout << "Enter quantity: ";
        cin >> pl.quantity;
        cout << "Enter price: ";
        cin >> pl.price;

        profitlosshistory.push(pl);
        while (!redoStack.empty()) redoStack.pop();

        if (pl.type == "Buy" || pl.type == "buy")
            totalInvestment += pl.quantity * pl.price;
        else if (pl.type == "Sell" || pl.type == "sell")
            totalProfitLoss += pl.quantity * pl.price;
        else
            cout << "Invalid Trade Type\n";

        cout << "✅ Trade recorded successfully.\n";
    }

    void undo() {
        if (profitlosshistory.empty()) {
            cout << "❌ Nothing to undo\n";
            return;
        }
        ProfitLoss last = profitlosshistory.top();
        profitlosshistory.pop();
        redoStack.push(last);

        if (last.type == "Buy" || last.type == "buy")
            totalInvestment -= last.quantity * last.price;
        else if (last.type == "Sell" || last.type == "sell")
            totalProfitLoss -= last.quantity * last.price;

        cout << "↩️ Undo successful\n";
    }

    void redo() {
        if (redoStack.empty()) {
            cout << "❌ Nothing to redo\n";
            return;
        }
        ProfitLoss pl = redoStack.top();
        redoStack.pop();
        profitlosshistory.push(pl);

        if (pl.type == "Buy" || pl.type == "buy")
            totalInvestment += pl.quantity * pl.price;
        else if (pl.type == "Sell" || pl.type == "sell")
            totalProfitLoss += pl.quantity * pl.price;

        cout << "↪️ Redo successful\n";
    }

    void DisplaySummary() {
        double net = totalProfitLoss - totalInvestment;
        cout << fixed << setprecision(2);
        cout << "\n===== PORTFOLIO SUMMARY =====\n";
        cout << "Total Investment : ₹" << totalInvestment << endl;
        cout << "Total Sell Value : ₹" << totalProfitLoss << endl;
        if (net > 0)
            cout << "📈 Net Profit : ₹" << net << endl;
        else if (net < 0)
            cout << "📉 Net Loss   : ₹" << -net << endl;
        else
            cout << "No Profit, No Loss\n";
    }

    void DisplayHistory() {
        if (profitlosshistory.empty()) {
            cout << "No trades yet.\n";
            return;
        }
        stack<ProfitLoss> temp = profitlosshistory;
        cout << "\n---- Trade History (latest first) ----\n";
        while (!temp.empty()) {
            ProfitLoss pl = temp.top();
            temp.pop();
            cout << pl.stockname << " | " << pl.type << " | "
                << pl.quantity << " | ₹" << pl.price << endl;
        }
    }

    void run() {
        LoadHistory();
        int choice;
        while (true) {
            cout << "\n===== STOCK TRACKER =====\n";
            cout << "1. Trade Stock\n";
            cout << "2. Undo Last Trade\n";
            cout << "3. Redo Last Trade\n";
            cout << "4. Display History\n";
            cout << "5. Display Summary\n";
            cout << "6. Save & Exit\n";
            cout << "Enter your choice: ";
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "❌ Invalid input.\n";
                continue;
            }
            if (choice == 6) break;
            switch (choice) {
                case 1: TradeStock(); break;
                case 2: undo(); break;
                case 3: redo(); break;
                case 4: DisplayHistory(); break;
                case 5: DisplaySummary(); break;
                default: cout << "❌ Invalid choice! Try again.\n";
            }
        }
        savehistory();
        cout << "💾 Data saved. Exiting...\n";
    }

} // namespace