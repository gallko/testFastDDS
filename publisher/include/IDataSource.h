#pragma once
#include <memory>
#include <ThreadBase.h>

template<typename DataType>
class IDataSource {
public:
    virtual ~IDataSource() = default;

    virtual void startCounting() = 0;
    virtual void stopCounting() = 0;
    virtual std::shared_ptr<DataType> popData() = 0;
    virtual std::string getSourceName() const = 0;
};
