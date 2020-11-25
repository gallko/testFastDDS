#include "DataGenerator.h"


DataGenerator::DataGenerator(const std::string &name, uint32_t timeOut, size_t sizePayload)
    : utils::ThreadBase(name + "_gen", timeOut)
    , mSizePayload(sizePayload)
{
    /* empty */
}

void DataGenerator::onLoop() {
    onDataAvailable(nullptr, mSizePayload);
}
