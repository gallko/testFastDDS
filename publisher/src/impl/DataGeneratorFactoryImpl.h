#pragma once

#include <memory>
#include <shared_mutex>
#include <map>

#include <IDataGeneratorFactory.h>

class DataGeneratorImpl;

class DataGeneratorFactoryImpl: public IDataGeneratorFactory {
public:
    DataGeneratorFactoryImpl();
    ~DataGeneratorFactoryImpl() override = default;

    DataGeneratorFactoryImpl(const DataGeneratorFactoryImpl&) = delete;
    DataGeneratorFactoryImpl &operator=(const DataGeneratorFactoryImpl&) = delete;
    DataGeneratorFactoryImpl(DataGeneratorFactoryImpl&&) = delete;
    DataGeneratorFactoryImpl &operator=(DataGeneratorFactoryImpl&&) = delete;

    std::shared_ptr<IDataGenerator> createGenerator(const std::string &name, int timeOut, size_t dataSize) override;
    std::shared_ptr<IDataGenerator> getGenerator(const std::string &name) override;
    void destroyGenerator(const std::string &name) override;
    void destroyAllGenerators() override;

private:
    friend IDataGeneratorFactory;
    static DataGeneratorFactoryImpl mInstance;

    std::shared_mutex mProtectDataGenerators;
    std::map<std::string, std::shared_ptr<DataGeneratorImpl>> mDataGenerators;
};
