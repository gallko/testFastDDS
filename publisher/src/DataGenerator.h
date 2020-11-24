#pragma once
#include <ThreadBase.h>

class DataGenerator : public utils::ThreadBase {
public:
    DataGenerator(const std::string &name, uint32_t timeOut);
    ~DataGenerator() override = default;

    virtual void onDataAvailable(void *sameData) = 0;

private:
    void onStart() override;
    void onLoop() override;
    void onClose() override;
};
