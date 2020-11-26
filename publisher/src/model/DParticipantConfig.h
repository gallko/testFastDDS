#pragma once
#include <string>

class DParticipantConfig {
public:
    DParticipantConfig(std::string name, const uint32_t timeOutLog)
        : mName(std::move(name)), mTimeOutLog(timeOutLog) {}
    const std::string mName;
    const uint32_t mTimeOutLog;
};

