// Minimal main that calls RealTimeTracker::run()
#include <iostream>
namespace RealTimeTracker { void run(); }
int main() {
    try { RealTimeTracker::run(); }
    catch (const std::exception& e) { std::cerr << "Fatal: " << e.what() << "\n"; return 1; }
    return 0;
}
