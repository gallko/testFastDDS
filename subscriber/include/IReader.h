#pragma once

#include <string>

class IReader {
public:
    virtual ~IReader() = default;

    virtual std::string getName() const = 0;
    virtual size_t getCountRecv() const = 0;
    virtual bool init() = 0;
    virtual bool haveConnection() = 0;
};