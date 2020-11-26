#include "Participant.h"
#include "impl/DataSourceImpl.h"
#include "impl/WriterMsg1_impl.h"
#include "impl/WriterMsg2_impl.h"

#include <AvailableDataTypes.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>

#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <ISignals.h>
#include <Report.h>

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
{
    /* empty */
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
    std::stringstream ss;
    for (auto const& [key, val]: mWriters) {
        auto msg_sent = val.first->getNumberMessagesSent();
        auto msg_gen = val.second->getContGen();
        ss << key << ": Sent: " << msg_sent << "; Gen: " << msg_gen << std::endl;
    }
    mSignals->sigPrintLogText(ss.str());
}

bool Participant::init() {
    DomainParticipantQos pqos;
    pqos.name(mName +"_pub");
    mDomainParticipant = DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    if (!mDomainParticipant) {
        return false;
    }
    AvailableDataTypes::instance().registerAllType(mDomainParticipant);

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

bool Participant::createWriter(const std::string &topicName, const std::string &dataName, size_t sizePayload, uint32_t timeToSend, uint32_t timeToGen) {
    std::unique_lock lock(mProtectedContainer);
    if (mWriters.find(topicName) != mWriters.end()) {
        return false;
    }
    mWriters[topicName] = {nullptr, nullptr};
    lock.unlock();

    if (!AvailableDataTypes::instance().isType(dataName)) {
        return false;
    }
    if (dataName == "msg1") {
        auto writer = std::make_shared<WriterMsg1Impl>(topicName, dataName, timeToSend);
        auto generator = std::make_shared<DataSourceImpl<Message_1>>(topicName, timeToGen, sizePayload, std::dynamic_pointer_cast<IDataConverter<Message_1>>(writer));
        if (!generator->init() || !writer->init(generator, mDomainParticipant, mPublisher)) {
            lock.lock();
            mWriters.erase(topicName);
            return false;
        }
        lock.lock();
        mWriters[topicName] = {writer, generator};
    } else if (dataName == "msg2") {
        auto writer = std::make_shared<WriterMsg2Impl>(topicName, dataName, timeToSend);
        auto generator = std::make_shared<DataSourceImpl<Message_2>>(topicName, timeToGen, sizePayload, std::dynamic_pointer_cast<IDataConverter<Message_2>>(writer));
        if (!generator->init() || !writer->init(generator, mDomainParticipant, mPublisher)) {
            lock.lock();
            mWriters.erase(topicName);
            return false;
        }
        lock.lock();
        mWriters[topicName] = {writer, generator};
    }
    return true;
}

void Participant::destroyWriter(const std::string &topicName) {
    std::unique_lock lock(mProtectedContainer);
    mWriters.erase(topicName);
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

