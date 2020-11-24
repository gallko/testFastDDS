#include "DataGenerator.h"


DataGenerator::DataGenerator(const std::string &name, uint32_t timeOut)
    : utils::ThreadBase(name, timeOut)
{
    /* empty */
}

void DataGenerator::onStart() {

}

void DataGenerator::onLoop() {
    onDataAvailable(nullptr);
}

void DataGenerator::onClose() {
}
