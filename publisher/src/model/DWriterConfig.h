#pragma once
#include <string>

class DWriterConfig {
public:
    DWriterConfig(std::string name, std::string dataType, const uint32_t sizePayload,
                  const uint32_t timeToGeneration, const uint32_t timeToSend)
    : mName(std::move(name))
    , mDataType(std::move(dataType))
    , mSizePayload(sizePayload)
    , mTimeToGeneration(timeToGeneration)
    , mTimeToSend(timeToSend) {}

    const std::string mName;
    const std::string mDataType;
    const uint32_t mSizePayload;
    const uint32_t mTimeToGeneration;
    const uint32_t mTimeToSend;
};
