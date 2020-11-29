#pragma once

#include <ThreadBase.h>

#include <atomic>
#include <vector>
#include <shared_mutex>

class DataGenerator : private utils::ThreadBase {
public:
    DataGenerator(const std::string &name, uint32_t timeOut);
    ~DataGenerator() override = default;

    virtual void onDataAvailable() = 0;
    void startGen();
    void stopGen();
    std::pair<bool, size_t> getStatusGen() const;
    virtual bool init();
    virtual std::string getName() const;

private:
    void onLoop() override;

    mutable std::shared_mutex mProtect;
    size_t mCountGen;
    bool isActive;
};
