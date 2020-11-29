#pragma once
#include <future>
#include <string>

class DParticipantConfig;
class DWriterConfig;
class DReaderConfig;

class ISignals {
public:
    virtual ~ISignals() = default;

    virtual std::future<void> sigPrintLogText(const std::string &message) = 0;
    virtual std::future<void> sigStopApp(int num = 0) = 0;
    virtual std::future<void> sigCreateParticipant(const DParticipantConfig &config) = 0;
    virtual std::future<void> sigCreateWriter(const DWriterConfig &config) = 0;
    virtual std::future<void> sigCreateReader(const DReaderConfig &config) = 0;
};
