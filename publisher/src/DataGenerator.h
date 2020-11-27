#pragma once
#include <atomic>
#include <ThreadBase.h>
#include <vector>

class DataGenerator : private utils::ThreadBase {
public:
    DataGenerator(const std::string &name, uint32_t timeOut);
    ~DataGenerator() override = default;

    virtual void onDataAvailable() = 0;
    void resetCountGen();
    size_t getContGen() const;
    virtual bool init();
    virtual std::string getName() const;

private:
    void onLoop() override;
    std::atomic<size_t> mCountGen;
};
