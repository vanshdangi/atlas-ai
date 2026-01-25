#include "tools/openAppTool.h"
#include <cstdlib>

std::string OpenAppTool::run(const std::string& input) {
#ifdef _WIN32
    int result = system(("start " + input).c_str());
    if (result != 0) {
        return "Could not open app: " + input;
    }

    return "Opened app: " + input;
#else
    return "OpenAppTool only implemented on Windows.";
#endif
}
