#include "tools/openAppTool.h"
#include <cstdlib>

std::string OpenAppTool::run(const std::string& input) {
#ifdef _WIN32
    std::string cmd = "start " + input;
    system(cmd.c_str());
    return "Opened " + input;
#else
    return "OpenAppTool only implemented on Windows.";
#endif
}
