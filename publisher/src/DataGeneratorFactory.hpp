#pragma once
/*
 * implementation
 * */

namespace {
    template<typename DataType>
    class DataSourceImpl: public IDataSource<DataType>, public DataGenerator {
    public:
        DataSourceImpl(const std::string &name, uint32_t timeOut, size_t sizePayload, std::weak_ptr<IDataConverter<DataType>> converter)
                : DataGenerator(name, timeOut, sizePayload)
                , mConverter(std::move(converter))
        {

        }
        ~DataSourceImpl() override = default;

        std::shared_ptr<DataType> popData() override {
            std::lock_guard lock(mProtectData);
            return std::move(mData);
        }

        std::string getSourceName() const override {
            return getNameThread();
        }

    private:
        void onDataAvailable(void *sameData, size_t sizePayload) override {
            if (auto converter = mConverter.lock(); converter) {
                auto data = converter->converter(sameData, sizePayload);
                std::lock_guard lock(mProtectData);
                mData = std::move(data);
            }
        }

        std::weak_ptr<IDataConverter<DataType>> mConverter;
        std::mutex mProtectData;
        std::shared_ptr<DataType> mData;
    };
}

template<typename DataType>
std::shared_ptr<IDataSource<DataType>>
DataGeneratorFactory::createGenerator(const std::string &name, uint32_t timeOut, size_t sizePayload, std::weak_ptr<IDataConverter<DataType>> converter)
{
    std::shared_ptr<DataSourceImpl<DataType>> ptr{nullptr};
    std::lock_guard lock(mProtectDataGenerators);
    if (!name.empty() && name.size() < 12) {
        auto item = mDataGenerators.find(name);
        if (item == mDataGenerators.end()) {
            ptr = std::make_shared<DataSourceImpl<DataType>>(name, timeOut, sizePayload, converter);
            if (ptr->initThread()) {
                mDataGenerators[name] = ptr;
            }
        }
    }
    return ptr;
}
