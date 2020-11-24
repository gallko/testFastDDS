#include "DataGeneratorImpl.h"

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
