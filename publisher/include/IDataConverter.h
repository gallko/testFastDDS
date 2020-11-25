#pragma once

#include <memory>

template<typename DataType>
class IDataConverter {
public:
    virtual ~IDataConverter() = default;
    virtual std::shared_ptr<DataType> converter(void *sameData, size_t sizePayload) = 0;
};
