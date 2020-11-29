#include "DataGenerator.h"


DataGenerator::DataGenerator(const std::string &name, uint32_t timeOut)
    : utils::ThreadBase(name + "_gen", timeOut)
    , mCountGen(0)
    , isActive(false)
{
    /* empty */
}

void DataGenerator::startGen() {
    std::unique_lock lock(mProtect);
    isActive = true;
}

void DataGenerator::stopGen() {
    std::unique_lock lock(mProtect);
    isActive = true;
    mCountGen = 0;
}

std::pair<bool, size_t> DataGenerator::getStatusGen() const {
    std::shared_lock lock(mProtect);
    return {isActive, mCountGen};
}
void DataGenerator::onLoop() {
    std::unique_lock lock(mProtect);
    if (isActive) {
        mCountGen++;
        lock.unlock();
        onDataAvailable();
    }
}

bool DataGenerator::init() {
    return initThread();
}

std::string DataGenerator::getName() const {
    return getNameThread();
}
