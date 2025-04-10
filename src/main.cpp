#include "ui.h"
#include <iostream>

int main() {
    try {
        // Create and run the scheduler UI
        ScheduleUI ui;
        ui.run();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
}
