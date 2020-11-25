#pragma once

class IWriter {
public:
    virtual ~IWriter() = default;

    virtual size_t getNumberMessagesSent() const = 0;
};