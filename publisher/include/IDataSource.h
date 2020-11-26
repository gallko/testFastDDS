#pragma once
#include <memory>
#include <ThreadBase.h>

template<typename DataType>
class IDataSource {
public:
    virtual ~IDataSource() = default;

    virtual std::shared_ptr<DataType> popData() = 0;
    virtual bool init() = 0;
    virtual std::string getSourceName() const = 0;
};
