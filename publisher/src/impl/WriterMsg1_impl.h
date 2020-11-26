#pragma once

#include <Message_1.h>
#include "../Writer.h"

class WriterMsg1Impl : public Writer<Message_1> {
public:
    WriterMsg1Impl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend);
    ~WriterMsg1Impl() override = default;
    std::shared_ptr<Message_1> converter(void *sameData, size_t sizePayload) override;
};
