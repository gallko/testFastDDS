#include "DataGenerator.h"


DataGenerator::DataGenerator(const std::string &name, uint32_t timeOut, size_t sizePayload)
    : utils::ThreadBase(name + "_gen", timeOut)
    , mSizePayload(sizePayload)
    , mCountGen()
{
    /* empty */
}


size_t DataGenerator::getContGen() const {
    return mCountGen.load();
}

void DataGenerator::onLoop() {
    mCountGen++;
    onDataAvailable(nullptr, mSizePayload);
}
