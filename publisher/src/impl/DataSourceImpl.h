#pragma once

#include <memory>
#include <functional>
#include <IDataConverter.h>
#include <IDataSource.h>
#include "../DataGenerator.h"

template<typename DataType>
class DataSourceImpl: public IDataSource<DataType>, public DataGenerator {
public:
    DataSourceImpl(const std::string &name, uint32_t timeOut, size_t sizePayload)
            : DataGenerator(name, timeOut)
            , mSizePayload(sizePayload)
    {
        /* empty */
    }
    ~DataSourceImpl() override = default;

    std::shared_ptr<DataType> popData() override {
        auto data = std::atomic_exchange(&mData, std::shared_ptr<DataType>(nullptr));
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
