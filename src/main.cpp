#include "ui.h"
#include <iostream>

int main()
{
    try
    {
        ScheduleUI().run();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "An unexpected error occurred." << std::endl;
    }
    return 1;
}