#include "Participant.h"

#include <AvailableDataTypes.h>
#include <ISignals.h>
#include <IReader.h>
#include "impl/ReaderImpl.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <Reports.h>
#include <Message_1.h>
#include <Message_2.h>

using namespace eprosima::fastdds::dds;

Participant::Participant(const std::string& name, uint32_t timeOutReport, std::shared_ptr<ISignals> signals)
    : ThreadBase(name + "_prt", timeOutReport)
    , mName(name)
    , mSignals(std::move(signals))
    , mDomainParticipant(nullptr)
    , mPublisher(nullptr)
    , mSubscriber(nullptr)
{
    /* empty */
}

Participant::~Participant() {
    mReaders.clear();
    clearDDS();
}

void Participant::clearDDS() {
    if (mDomainParticipant) {
        if (mPublisher) {
            mDomainParticipant->delete_publisher(mPublisher);
            mPublisher = nullptr;
        }

        if (mSubscriber) {
            mDomainParticipant->delete_subscriber(mSubscriber);
            mSubscriber = nullptr;
        }

        DomainParticipantFactory::get_instance()->delete_participant(mDomainParticipant);
        mDomainParticipant = nullptr;
    }
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

    return initThread();
}

void Participant::onLoop() {
    std::stringstream ss;
    Reports reports;
    std::shared_lock lock(mProtectReaders);
    for (const auto &[name, reader]: mReaders) {
        if (!reader->haveConnection()) {
            continue;
        }
        ReportTopic r;
        r.name(name);
        r.number_of_packets(reader->getCountRecv());
        reports.data().push_back(std::move(r));
    }
    if (!reports.data().empty()) {
        mSignals->sigPrintLogText("Time to send report\n");
    }
}

bool Participant::createReader(const std::string &topicName, const std::string &typeName) {
    std::unique_lock lock(mProtectReaders);
    if (mReaders.find(topicName) != mReaders.end()) {
        return false;
    }
    std::shared_ptr<IReader> reader(nullptr);

    if (typeName == "msg1") {
        reader = std::make_shared<ReaderImpl<Message_1>>([](const Message_1 &msg) {}, topicName, typeName, mDomainParticipant, mSubscriber);
    } else if (typeName == "msg2") {
        reader = std::make_shared<ReaderImpl<Message_2>>([](const Message_2 &msg) {}, topicName, typeName, mDomainParticipant, mSubscriber);
    }

    if (!reader->init()) {
        return false;
    }
    mReaders[topicName] = reader;
    return true;
}
