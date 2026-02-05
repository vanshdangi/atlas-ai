#include "utils/utils.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

void trim_and_normalize(std::string& s) {
    // 1. Trim leading whitespace
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    // 2. Trim trailing whitespace
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    // 3. Convert to lowercase
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return std::tolower(ch);
    });

    // 4. Remove punctuation (keep letters, digits, spaces)
    s.erase(
        std::remove_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isalnum(ch) && !std::isspace(ch);
        }),
        s.end()
    );

    // 5. Trim again (in case punctuation removal created edges)
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string generate_id() {
    static std::atomic<uint64_t> counter{0};
    return "mem_" + std::to_string(++counter);
}

std::string current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}