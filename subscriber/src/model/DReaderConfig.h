#pragma once
#include <string>

class DReaderConfig {
public:
    DReaderConfig(std::string name, std::string dataType)
    : mName(std::move(name))
    , mDataType(std::move(dataType)) {}

    const std::string mName;
    const std::string mDataType;
};
