#pragma once
#include <future>
#include <string>

class DConfigSender;
class DConfigReceiver;

class ISignals {
public:
    virtual ~ISignals() = default;

    virtual std::future<void> sigPrintLogText(const std::string &message) = 0;
    virtual std::future<void> sigStopApp(int num = 0) = 0;
    virtual std::future<void> sigAddSender(const DConfigSender &config) = 0;
    virtual std::future<void> sigAddReceiver(const DConfigReceiver &config) = 0;
};
