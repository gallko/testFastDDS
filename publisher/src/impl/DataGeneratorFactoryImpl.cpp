#include <atomic>
#include <iostream>
#include <IDataGeneratorFactory.h>

#include "DataGeneratorFactoryImpl.h"
#include "DataGeneratorImpl.h"

DataGeneratorFactoryImpl DataGeneratorFactoryImpl::mInstance{};

std::shared_ptr<IDataGeneratorFactory> IDataGeneratorFactory::instance() {
    static auto ptr = std::shared_ptr<IDataGeneratorFactory>(&DataGeneratorFactoryImpl::mInstance, [](void *){});
    return ptr;
}

DataGeneratorFactoryImpl::DataGeneratorFactoryImpl()
    : mProtectDataGenerators()
    , mDataGenerators()
{
    /* empty */
}

std::shared_ptr<IDataGenerator>
DataGeneratorFactoryImpl::createGenerator(const std::string &name, int timeOut, size_t dataSize) {
    std::shared_ptr<DataGeneratorImpl> ptr{nullptr};
    std::lock_guard lock(mProtectDataGenerators);
    if (!name.empty() && name.size() < 16) {
        auto item = mDataGenerators.find(name);
        if (item == mDataGenerators.end()) {
            ptr = std::make_shared<DataGeneratorImpl>(name, timeOut, dataSize);
            if (ptr->initThread()) {
                mDataGenerators[name] = ptr;
            }
        }
    }
    return ptr;
}

std::shared_ptr<IDataGenerator> DataGeneratorFactoryImpl::getGenerator(const std::string &name) {
    std::shared_lock lock(mProtectDataGenerators);
    auto item = mDataGenerators.find(name);
    return item != mDataGenerators.end() ? item->second : nullptr;
}

void DataGeneratorFactoryImpl::destroyGenerator(const std::string &name) {
    std::unique_lock lock(mProtectDataGenerators);
    auto item = mDataGenerators.find(name);
    if (item != mDataGenerators.end()) {
        auto ptr = item->second;
        mDataGenerators.erase(item);
        lock.unlock();
        ptr->closeThread();
    }
}

void DataGeneratorFactoryImpl::destroyAllGenerators() {
    std::lock_guard lock(mProtectDataGenerators);
    for (auto &el: mDataGenerators) {
        el.second->closeThread();
    }
    mDataGenerators.clear();
}
