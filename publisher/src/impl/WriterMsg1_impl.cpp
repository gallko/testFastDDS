#include "WriterMsg1_impl.h"
#include <string>

WriterMsg1Impl::WriterMsg1Impl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
    : Writer<Message_1>(topicName, typeName, timeOutToSend)
{
    /* empty */
}

std::shared_ptr<Message_1> WriterMsg1Impl::converter(void *sameData, size_t sizePayload) {

    auto msg = std::make_shared<Message_1>();
    msg->message(std::string(sizePayload, 'a'));
    return msg;
}
