#pragma once
#include <memory>
#include <map>
#include <string>
#include <shared_mutex>
#include <ThreadBase.h>
#include <IDataGenerator.h>

class DataGeneratorImpl : public IDataGenerator, public utils::ThreadBase {
private: // static
    static std::shared_ptr<char[]> genData(size_t dataSize);

public:
    DataGeneratorImpl(const std::string &name, uint32_t timeOut, size_t dataSize);
    ~DataGeneratorImpl() override = default;

    std::shared_ptr<char[]> popData() override;
    std::string getName() const override;

    DataGeneratorImpl(const DataGeneratorImpl&) = delete;
    DataGeneratorImpl &operator=(const DataGeneratorImpl&) = delete;
    DataGeneratorImpl(DataGeneratorImpl&&) = delete;
    DataGeneratorImpl &operator=(DataGeneratorImpl&&) = delete;

private:
    void onStart() override;
    void onLoop() override;
    void onClose() override;

private:
    std::mutex mLockData;
    size_t mDataSize;
    std::shared_ptr<char[]> mData;
};
