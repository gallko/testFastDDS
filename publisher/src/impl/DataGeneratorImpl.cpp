#include "DataGeneratorImpl.h"

std::shared_mutex DataGeneratorImpl::mProtectDataGenerators {};
std::map<std::string, std::shared_ptr<DataGeneratorImpl>> DataGeneratorImpl::mDataGenerators {};

std::shared_ptr<IDataGenerator> DataGeneratorFactory::createGenerator(const std::string &name, int timeOut, size_t dataSize) {
    std::shared_ptr<DataGeneratorImpl> ptr{nullptr};
    std::lock_guard lock(DataGeneratorImpl::mProtectDataGenerators);
    if (!name.empty() && name.size() < 16) {
        auto item = DataGeneratorImpl::mDataGenerators.find(name);
        if (item == DataGeneratorImpl::mDataGenerators.end()) {
            ptr = std::make_shared<DataGeneratorImpl>(name, timeOut, dataSize);
            if (ptr->initThread()) {
                DataGeneratorImpl::mDataGenerators[name] = ptr;
            }
        }
    }
    return ptr;
}

std::shared_ptr<IDataGenerator> DataGeneratorFactory::getGenerator(const std::string &name) {
    std::shared_lock lock(DataGeneratorImpl::mProtectDataGenerators);
    auto item = DataGeneratorImpl::mDataGenerators.find(name);
    return item != DataGeneratorImpl::mDataGenerators.end() ? item->second : nullptr;
}

void DataGeneratorFactory::destroyGenerator(const std::string &name) {
    std::unique_lock lock(DataGeneratorImpl::mProtectDataGenerators);
    auto item = DataGeneratorImpl::mDataGenerators.find(name);
    if (item != DataGeneratorImpl::mDataGenerators.end()) {
        auto ptr = item->second;
        DataGeneratorImpl::mDataGenerators.erase(item);
        lock.unlock();
        ptr->closeThread();
    }
}

void DataGeneratorFactory::destroyAllGenerators() {
    std::lock_guard lock(DataGeneratorImpl::mProtectDataGenerators);
    for (auto &el: DataGeneratorImpl::mDataGenerators) {
        el.second->closeThread();
    }
    DataGeneratorImpl::mDataGenerators.clear();
}

std::shared_ptr<char[]> DataGeneratorImpl::genData(size_t dataSize) {
    auto ptr = std::shared_ptr<char[]>(new char[dataSize]);
    for (int i = 0; i < dataSize; ++i) {
        ptr[i] = 'a';
    }
    return ptr;
}

std::shared_ptr<char[]> DataGeneratorImpl::popData() {
    auto data = std::move(mData);
    return data;
}

DataGeneratorImpl::DataGeneratorImpl(const std::string &name, uint32_t timeOut, size_t dataSize)
    : utils::ThreadBase(name, timeOut)
    , mLockData()
    , mDataSize(dataSize)
    , mData(nullptr)
{
    /* empty */
}

void DataGeneratorImpl::onStart() {

}

void DataGeneratorImpl::onLoop() {
    std::lock_guard lock(mLockData);
    mData = genData(mDataSize);
}

void DataGeneratorImpl::onClose() {
    std::lock_guard lock(mLockData);
    mData.reset();
}

std::string DataGeneratorImpl::getName() const {
    return getNameThread();
}
