#pragma once

#include <memory>
#include <shared_mutex>
#include <map>
#include <IDataSource.h>
#include <IDataConverter.h>
#include "DataGenerator.h"

class DataGeneratorFactory {
public:
    static std::shared_ptr<DataGeneratorFactory> instance();
    ~DataGeneratorFactory() = default;

    DataGeneratorFactory(const DataGeneratorFactory&) = delete;
    DataGeneratorFactory &operator=(const DataGeneratorFactory&) = delete;
    DataGeneratorFactory(DataGeneratorFactory&&) = delete;
    DataGeneratorFactory &operator=(DataGeneratorFactory&&) = delete;

    template<typename DataType>
    std::shared_ptr<IDataSource<DataType>> createGenerator(const std::string &name, int timeOut, std::shared_ptr<IDataConverter<DataType>> converter);

    void destroyGenerator(const std::string &name);
    void destroyAllGenerators();

protected:
    DataGeneratorFactory();

private:
    static DataGeneratorFactory mInstance;

    std::shared_mutex mProtectDataGenerators;
    std::map<std::string, std::shared_ptr<utils::ThreadBase>> mDataGenerators;
};

#include "DataGeneratorFactory.hpp"