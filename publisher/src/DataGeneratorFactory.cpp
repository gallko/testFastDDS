#include <atomic>
#include <iostream>

#include "DataGeneratorFactory.h"
#include "DataGenerator.h"

DataGeneratorFactory DataGeneratorFactory::mInstance{};

std::shared_ptr<DataGeneratorFactory> DataGeneratorFactory::instance() {
    static auto ptr = std::shared_ptr<DataGeneratorFactory>(&DataGeneratorFactory::mInstance, [](void *){});
    return ptr;
}

DataGeneratorFactory::DataGeneratorFactory()
    : mProtectDataGenerators()
    , mDataGenerators()
{
    /* empty */
}

void DataGeneratorFactory::destroyGenerator(const std::string &name) {
    std::unique_lock lock(mProtectDataGenerators);
    auto item = mDataGenerators.find(name);
    if (item != mDataGenerators.end()) {
        auto ptr = item->second;
        mDataGenerators.erase(item);
        lock.unlock();
        ptr->closeThread();
    }
}

void DataGeneratorFactory::destroyAllGenerators() {
    std::lock_guard lock(mProtectDataGenerators);
    for (auto &el: mDataGenerators) {
        el.second->closeThread();
    }
    mDataGenerators.clear();
}
