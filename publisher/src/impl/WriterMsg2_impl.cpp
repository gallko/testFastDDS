#include "WriterMsg2_impl.h"
#include <string>

WriterMsg2Impl::WriterMsg2Impl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
        : Writer<Message_2>(topicName, typeName, timeOutToSend)
{
    /* empty */
}

std::shared_ptr<Message_2> WriterMsg2Impl::converter(void *sameData, size_t sizePayload) {
    Message_2 d;
    auto msg = std::make_shared<Message_2>();
    msg->data(std::vector<int32_t>(sizePayload,500));
    return msg;
}
