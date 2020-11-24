#pragma once

template<typename DataType>
class IDataConverter {
public:
    virtual ~IDataConverter() = default;
    virtual std::shared_ptr<DataType> converter(void *sameData) = 0;
};
