// portfolio_analyzer_main.cpp
#include <iostream>

namespace PortfolioAnalyzer {
    void run();  // forward declaration
}

int main() {
    try {
        PortfolioAnalyzer::run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Fatal: Unknown error occurred.\n";
        return 1;
    }
    return 0;
}
