#pragma once
#include <atomic>
#include <ThreadBase.h>

class DataGenerator : public utils::ThreadBase {
public:
    DataGenerator(const std::string &name, uint32_t timeOut, size_t sizePayload);
    ~DataGenerator() override = default;

    virtual void onDataAvailable(void *sameData, size_t size) = 0;
    virtual size_t getContGen() const;

private:
    void onLoop() override;
    size_t mSizePayload;
    std::atomic<size_t> mCountGen;
};
