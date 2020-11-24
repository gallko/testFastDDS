#pragma once
/*
 * implementation
 * */

namespace {
    template<typename DataType>
    class DataSourceImpl: public IDataSource<DataType>, public DataGenerator {
    public:
        DataSourceImpl(const std::string &name, uint32_t timeOut, std::shared_ptr<IDataConverter<DataType>> converter)
                : DataGenerator(name, timeOut)
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
        void onDataAvailable(void *sameData) override {
            auto data = mConverter->converter(sameData);
            std::lock_guard lock(mProtectData);
            mData = std::move(data);
        }

        std::shared_ptr<IDataConverter<DataType>> mConverter;
        std::mutex mProtectData;
        std::shared_ptr<DataType> mData;
    };
}

template<typename DataType>
std::shared_ptr<IDataSource<DataType>>
DataGeneratorFactory::createGenerator(const std::string &name, int timeOut, std::shared_ptr<IDataConverter<DataType>> converter)
{
    std::shared_ptr<DataSourceImpl<DataType>> ptr{nullptr};
    std::lock_guard lock(mProtectDataGenerators);
    if (converter && !name.empty() && name.size() < 16) {
        auto item = mDataGenerators.find(name);
        if (item == mDataGenerators.end()) {
            ptr = std::make_shared<DataSourceImpl<DataType>>(name, timeOut, converter);
            if (ptr->initThread()) {
                mDataGenerators[name] = ptr;
            }
        }
    }
    return ptr;
}
