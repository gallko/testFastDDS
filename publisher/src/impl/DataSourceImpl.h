#pragma once

#include <memory>
#include <functional>
#include <IDataSource.h>
#include "../DataGenerator.h"

template<typename DataType>
class DataSourceImpl: public IDataSource<DataType>, public DataGenerator {
public:
    DataSourceImpl(const std::string &topicName, uint32_t timeOut, size_t sizePayload)
            : DataGenerator(topicName, timeOut)
            , mSizePayload(sizePayload)
    {
        /* empty */
    }
    ~DataSourceImpl() override = default;

    std::shared_ptr<DataType> popData() override {
        auto status = getStatusGen().first;
        auto data = std::atomic_exchange(&mData, std::shared_ptr<DataType>(nullptr));
        if (status)
        return data;
    }

    bool init(std::function< std::shared_ptr<DataType>(size_t) > data) {
        // time to init source
        mDataSource = std::move(data);
        return DataGenerator::init();
    }

    std::string getSourceName() const override {
        return DataGenerator::getName();
    }

    void startCounting() override {
        startGen();
    }

    void stopCounting() override {
        std::atomic_store(&mData, std::shared_ptr<DataType>());
        stopGen();
    }

private:
    void onDataAvailable() override {
        // read data from any source
        auto data = mDataSource(mSizePayload);
        std::atomic_store(&mData, data);
    }

    std::shared_ptr<DataType> mData;
    const size_t mSizePayload;
    std::function<std::shared_ptr<DataType>(size_t)> mDataSource;
};
