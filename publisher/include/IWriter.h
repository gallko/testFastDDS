#pragma once

class IWriter {
public:
    virtual ~IWriter() = default;

    virtual void startCount() = 0;
    virtual void stopCount() = 0;
    virtual std::pair<bool, size_t> getCountStatus() const = 0;
};