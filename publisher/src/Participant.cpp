#include "Participant.h"
#include "WriterMsg1.h"
#include "DataGeneratorFactory.h"

#include <Message_1.h>
#include <Message_1PubSubTypes.h>
#include <ReportPubSubTypes.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>

#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <ISignals.h>

using namespace eprosima::fastdds::dds;

Participant::Participant(const std::string& name, uint32_t timeOutLog, std::shared_ptr<ISignals> signals)
    : ThreadBase(name + "_prt", timeOutLog)
    , mName(name)
    , mSignals(std::move(signals))
    , mDomainParticipant(nullptr)
    , mPublisher(nullptr)
    , mSubscriber(nullptr)
    , mTopicReport(nullptr)
    , mReaderReport(nullptr)
    , mTypeSupportMsg1(new Message_1PubSubType())
    , mTypeSupportReport(new ReportPubSubType())
{

}

Participant::~Participant() {
    mWriters.clear();
    clearDDS();
}

void Participant::clearDDS() {
    if (mDomainParticipant) {
        if (mPublisher) {
            mDomainParticipant->delete_publisher(mPublisher);
            mPublisher = nullptr;
        }

        if (mSubscriber) {
            if (mTopicReport) {
                if (mReaderReport) {
                    mSubscriber->delete_datareader(mReaderReport);
                    mReaderReport = nullptr;
                }
                mDomainParticipant->delete_topic(mTopicReport);
                mTopicReport = nullptr;
            }
            mDomainParticipant->delete_subscriber(mSubscriber);
            mSubscriber = nullptr;
        }

        DomainParticipantFactory::get_instance()->delete_participant(mDomainParticipant);
        mDomainParticipant = nullptr;
    }
}

void Participant::onLoop() {
    mSignals->sigPrintLogText("From Participant: report\n");
}

bool Participant::init() {
    DomainParticipantQos pqos;
    pqos.name(mName +"_pub");
    mDomainParticipant = DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    if (!mDomainParticipant) {
        return false;
    }
    mTypeSupportMsg1.register_type(mDomainParticipant, "msg1");
    mTypeSupportReport.register_type(mDomainParticipant, "report");

    mPublisher = mDomainParticipant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
    if (!mPublisher) {
        clearDDS();
        return false;
    }

    mSubscriber = mDomainParticipant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
    if (!mPublisher) {
        clearDDS();
        return false;
    }

    mTopicReport = mDomainParticipant->create_topic("reportTopic","report", TOPIC_QOS_DEFAULT);
    if (!mTopicReport) {
        clearDDS();
        return false;
    }

    mReaderReport = mSubscriber->create_datareader(mTopicReport, DATAREADER_QOS_DEFAULT, this);
    if (!mReaderReport) {
        clearDDS();
        return false;
    }

    initThread();
    return true;
}

bool Participant::creatWriter(const std::string &topicName, const std::string &typeName, size_t sizePayload, uint32_t timeOutToSend, uint32_t timeOutToGen) {
    auto writer = std::make_shared<WriterMsg1>(topicName, typeName, timeOutToSend);
    auto generator = DataGeneratorFactory::instance()->createGenerator<Message_1>(topicName, timeOutToGen, sizePayload, writer);
    writer->init(generator, mDomainParticipant, mPublisher);
    mWriters[topicName] = writer;
    return true;
}

void Participant::on_data_available(DataReader *reader) {
    SampleInfo info;
    Report r;
    while (reader->take_next_sample(&r, &info) == ReturnCode_t::RETCODE_OK)
    {
        if (info.instance_state == ALIVE) {
            std::cout << "Report: " << r.number_of_packets() << std::endl;
        }
    }
}

