#pragma once
#include <memory>

class IDataGenerator {
public:
    virtual ~IDataGenerator() = default;
    virtual std::shared_ptr<char[]> popData() = 0;
    virtual std::string getName() const = 0;
};
