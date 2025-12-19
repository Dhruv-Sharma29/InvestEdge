// Minimal main that calls StockNews::run()
#include <iostream>
namespace StockNews { void run(); }
int main() {
    try { StockNews::run(); }
    catch (const std::exception& e) { std::cerr << "Fatal: " << e.what() << "\n"; return 1; }
    return 0;
}
