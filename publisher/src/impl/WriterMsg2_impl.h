#pragma once

#include <Message_2.h>
#include "../Writer.h"

class WriterMsg2Impl : public Writer<Message_2> {
public:
    WriterMsg2Impl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend);
    ~WriterMsg2Impl() override = default;
    std::shared_ptr<Message_2> converter(void *sameData, size_t sizePayload) override;
};

