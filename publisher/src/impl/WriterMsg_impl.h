#pragma once
/*
#include <Message_1.h>
#include <Message_2.h>
#include "../Writer.h"

template<typename DataType>
class WriterMsgImpl;

template<>
class WriterMsgImpl<Message_1> : public Writer<Message_1> {
public:
    WriterMsgImpl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
            : Writer<Message_1>(topicName, typeName, timeOutToSend) {};
    ~WriterMsgImpl() override = default;
};

template<>
class WriterMsgImpl<Message_2> : public Writer<Message_2> {
public:
    WriterMsgImpl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
            : Writer<Message_2>(topicName, typeName, timeOutToSend) {};
    ~WriterMsgImpl() override = default;
    std::shared_ptr<Message_2> converter(const std::vector<char> &sameData) override {
        Message_2 d;
        auto msg = std::make_shared<Message_2>();
        msg->data(std::vector<int32_t>(sameData.size(),500));
        return msg;
    };
};
*/