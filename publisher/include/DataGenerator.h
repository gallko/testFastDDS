#pragma once
#include <memory>

class IDataGenerator;

class DataGeneratorFactory {
public:
    static std::shared_ptr<IDataGenerator> createGenerator(const std::string &name, int timeOut, size_t dataSize);
    static std::shared_ptr<IDataGenerator> getGenerator(const std::string &name);
    static void destroyGenerator(const std::string &name);
    static void destroyAllGenerators();
};

class IDataGenerator {
public:
    virtual ~IDataGenerator() = default;
    virtual std::shared_ptr<char[]> popData() = 0;
    virtual std::string getName() const = 0;
};
