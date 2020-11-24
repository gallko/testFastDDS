#pragma once
#include <memory>

class IDataGenerator;

class IDataGeneratorFactory {
public:
    virtual ~IDataGeneratorFactory() = default;
    static std::shared_ptr<IDataGeneratorFactory> instance();

    virtual std::shared_ptr<IDataGenerator> createGenerator(const std::string &name, int timeOut, size_t dataSize) = 0;
    virtual std::shared_ptr<IDataGenerator> getGenerator(const std::string &name) = 0;

    virtual void destroyGenerator(const std::string &name) = 0;
    virtual void destroyAllGenerators() = 0;
};
