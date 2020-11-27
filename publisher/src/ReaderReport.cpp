#include "ReaderReport.h"
#include <ISignals.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <Report.h>


ReaderReport::ReaderReport(const std::string &topicName, const std::string &typeName,
                           std::shared_ptr<ISignals> signals)
 : mSignals(std::move(signals))
 , mTypeName(typeName)
 , mTopicName(topicName)
 , mDomainParticipant(nullptr)
 , mSubscriber(nullptr)
 , mDataReader(nullptr)
 , mTopic(nullptr)
{

}

ReaderReport::~ReaderReport() {
    if (mDataReader)
    {
        mSubscriber->delete_datareader(mDataReader);
    }
    if (mTopic)
    {
        mDomainParticipant->delete_topic(mTopic);
    }
}

void ReaderReport::on_data_available(eprosima::fastdds::dds::DataReader *reader) {
    using namespace eprosima::fastdds::dds;
    SampleInfo info;
    Report msg;
    if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK) {
        if (info.instance_state == ALIVE)
        {
            std::stringstream ss;
            ss << "Report from " << msg.name() << ": " << "received " << msg.number_of_packets() << " messages\n";
            mSignals->sigPrintLogText(ss.str());
        }
    }
}

bool ReaderReport::init(eprosima::fastdds::dds::DomainParticipant *domainParticipant,
                        eprosima::fastdds::dds::Subscriber *subscriber) {
    using namespace eprosima::fastdds::dds;
    if (!domainParticipant || !subscriber) {
        return false;
    }

    mDomainParticipant = domainParticipant;
    mSubscriber = subscriber;
    mTopic = mDomainParticipant->create_topic(mTopicName, mTypeName, TOPIC_QOS_DEFAULT);
    if (mTopic == nullptr) {
        return false;
    }
    mDataReader = mSubscriber->create_datareader(mTopic, DATAREADER_QOS_DEFAULT, this);

    if (mDataReader == nullptr) {
        mDomainParticipant->delete_topic(mTopic);
        mTopic = nullptr;
        return false;
    }

    return true;
}
