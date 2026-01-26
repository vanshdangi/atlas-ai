#pragma once
#include <string>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class Tool {
public:
    virtual ~Tool() {}

    virtual std::string name() const = 0;

    virtual std::string description() const = 0;

    virtual std::string run(const json& args) = 0;
};
