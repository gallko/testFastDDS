#include "DataGenerator.h"


DataGenerator::DataGenerator(const std::string &name, uint32_t timeOut)
    : utils::ThreadBase(name + "_gen", timeOut)
    , mCountGen(0)
{
    /* empty */
}

void DataGenerator::resetCountGen() {
    mCountGen.store(0);
}

size_t DataGenerator::getContGen() const {
    return mCountGen.load();
}

void DataGenerator::onLoop() {
    mCountGen++;
    onDataAvailable();
}

bool DataGenerator::init() {
    return initThread();
}

std::string DataGenerator::getName() const {
    return getNameThread();
}
