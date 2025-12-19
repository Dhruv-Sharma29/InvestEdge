// Minimal main that calls RealTimeTrackerWithRisk::run()
#include <iostream>

namespace RealTimeTrackerWithRisk {
    void run();   // forward declaration of your run() function
}

int main() {
    try {
        RealTimeTrackerWithRisk::run();
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
