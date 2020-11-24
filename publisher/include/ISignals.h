#pragma once

class ISignals {
public:
    virtual ~ISignals() = default;

    virtual void sigPrintLogText(const std::string &message) = 0;
    virtual void sigStopApp(int num = 0) = 0;
};
