#include "tools/shutdownTool.h"
#include <cstdlib>

std::string ShutdownTool::run(const json&) {
#ifdef _WIN32
    system("shutdown /s /t 5");
    return "Shutting down in 5 seconds...";
#else
    return "ShutdownTool only works on Windows.";
#endif
}
