#pragma once

class DConfigSender;
class DConfigReceiver;

class ISignals {
public:
    virtual ~ISignals() = default;

    virtual void sigPrintLogText(const std::string &message) = 0;
    virtual void sigStopApp(int num = 0) = 0;
    virtual void sigAddSender(const DConfigSender &config) = 0;
    virtual void sigAddReceiver(const DConfigReceiver &config) = 0;
};
