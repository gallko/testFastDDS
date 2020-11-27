#pragma once

#include <memory>
#include <string>
#include <atomic>

#include <fastdds/dds/subscriber/DataReaderListener.hpp>

namespace eprosima::fastdds::dds {
    class DomainParticipant;
    class Subscriber;
    class DataReader;
    class Topic;
}
class ISignals;

class ReaderReport: public eprosima::fastdds::dds::DataReaderListener {
public:
    ReaderReport(const std::string &topicName, const std::string &typeName, std::shared_ptr<ISignals> signals);
    ~ReaderReport() override;
    bool init(eprosima::fastdds::dds::DomainParticipant* domainParticipant, eprosima::fastdds::dds::Subscriber* subscriber);

private:
    void on_data_available(eprosima::fastdds::dds::DataReader *reader) override;

    std::shared_ptr<ISignals> mSignals;
    const std::string mTypeName;
    const std::string mTopicName;
    eprosima::fastdds::dds::DomainParticipant* mDomainParticipant;
    eprosima::fastdds::dds::Subscriber* mSubscriber;
    eprosima::fastdds::dds::DataReader* mDataReader;
    eprosima::fastdds::dds::Topic* mTopic;
};