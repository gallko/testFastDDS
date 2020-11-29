#include "Participant.h"

#include <AvailableDataTypes.h>
#include <ISignals.h>
#include <IReader.h>
#include "impl/ReaderImpl.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <Reports.h>
#include <Message_1.h>
#include <Message_2.h>

using namespace eprosima::fastdds::dds;

Participant::Participant(const std::string& name, uint32_t timeOutReport, std::shared_ptr<ISignals> signals)
    : mName(name)
    , mSignals(std::move(signals))
    , mWriterReport(std::make_shared<WriterImpl<Reports>>("Report", "report", timeOutReport))
    , mDomainParticipant(nullptr)
    , mPublisher(nullptr)
    , mSubscriber(nullptr)
{
    /* empty */
}

Participant::~Participant() {
    mWriterReport.reset();
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

    mWriterReport->init(shared_from_this(), mDomainParticipant, mPublisher);
    if (!mWriterReport) {
        clearDDS();
        return false;
    }

    mWriterReport->startCount();

    return true;
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

void Participant::startCounting() {

}

void Participant::stopCounting() {

}

std::shared_ptr<Reports> Participant::popData() {
    std::stringstream ss;
    std::shared_ptr<Reports> reports{nullptr};

    std::shared_lock lock(mProtectReaders);
    if (!mReaders.empty()) {
        reports = std::make_shared<Reports>();
        for (const auto &[name, reader]: mReaders) {
            if (!reader->haveConnection()) {
                continue;
            }
            ReportTopic r;
            r.name(name);
            r.number_of_packets(reader->getCountRecv());
            reports->data().push_back(std::move(r));
        }
    }
    return reports;
}

std::string Participant::getSourceName() const {
    return mName;
}
