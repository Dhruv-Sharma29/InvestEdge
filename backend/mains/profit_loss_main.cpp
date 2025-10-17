// Minimal main that calls ProfitLossModule::run()
#include <iostream>
namespace ProfitLossModule { void run(); }
int main() {
    try { ProfitLossModule::run(); }
    catch (const std::exception& e) { std::cerr << "Fatal: " << e.what() << "\n"; return 1; }
    return 0;
}
