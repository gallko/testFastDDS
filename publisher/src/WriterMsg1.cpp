#include "WriterMsg1.h"

WriterMsg1::WriterMsg1(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
    : Writer<Message_1>(topicName, typeName, timeOutToSend)
{
    /* empty */
}

std::shared_ptr<Message_1> WriterMsg1::converter(void *sameData, size_t sizePayload) {
    return std::shared_ptr<Message_1>();
}
