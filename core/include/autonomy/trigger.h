#pragma once
#include <string>

class Trigger {
public:
    virtual ~Trigger() = default;

    virtual void poll() = 0;
    virtual bool fired() const = 0;

    // Returns type string for task factory
    virtual std::string triggerType() const = 0;

    virtual void reset() = 0;
};
