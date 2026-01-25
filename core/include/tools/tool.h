#pragma once
#include <string>

class Tool {
public:
    virtual ~Tool() {}

    virtual std::string name() const = 0;

    virtual std::string description() const = 0;

    virtual std::string run(const std::string& input) = 0;
};
