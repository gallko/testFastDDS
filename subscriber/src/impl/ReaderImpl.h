#pragma once

#include <functional>
#include <string>
#include <shared_mutex>

#include <IReader.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

template <typename DataType>
class ReaderImpl : public IReader, public eprosima::fastdds::dds::DataReaderListener {
public:
    ReaderImpl(std::function<void(const DataType&)> worker
               , const std::string &topicName
               , const std::string &typeName
               , eprosima::fastdds::dds::DomainParticipant* participant
               , eprosima::fastdds::dds::Subscriber* subscriber
    );
    ~ReaderImpl() override;

    std::string getName() const override;
    size_t getCountRecv() const override;
    bool init() override;
    bool haveConnection() override;

    void on_data_available(eprosima::fastdds::dds::DataReader *reader) override;
    void on_subscription_matched(eprosima::fastdds::dds::DataReader *reader, const eprosima::fastdds::dds::SubscriptionMatchedStatus &info) override;

private:
    std::function<void(const DataType&)> mWorker;
    const std::string mTopicName;
    const std::string mTypeName;

    eprosima::fastdds::dds::DomainParticipant* mDomainParticipant;
    eprosima::fastdds::dds::Subscriber* mSubscriber;
    eprosima::fastdds::dds::DataReader* mDataReader;
    eprosima::fastdds::dds::Topic* mTopic;

    mutable std::shared_mutex mProtectCount;
    size_t mCountRecv;
    bool mHaveConnection;

    DataType mMsg;
    int mMatched;
};

#include "ReaderImpl.hpp"