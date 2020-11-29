#include "Participant.h"
#include "impl/DataSourceImpl.h"
#include "impl/WriterImpl.h"
#include "ReaderReport.h"

#include <AvailableDataTypes.h>
#include <ISignals.h>
#include <Message_1.h>
#include <Message_2.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

using namespace eprosima::fastdds::dds;

namespace {
    std::shared_ptr<Message_1> genMsg1(size_t sizePayload) {
        auto ptr = std::make_shared<Message_1>();
        ptr->message(std::string(sizePayload, 'a'));
        return ptr;
    }
    std::shared_ptr<Message_2> genMsg2(size_t sizePayload) {
        auto ptr = std::make_shared<Message_2>();
        ptr->data(std::vector<int32_t>(sizePayload, 100));
        return ptr;
    }
}

Participant::Participant(const std::string& name, uint32_t timeOutLog, std::shared_ptr<ISignals> signals)
    : ThreadBase(name + "_prt", timeOutLog)
    , mName(name)
    , mSignals(std::move(signals))
    , mDomainParticipant(nullptr)
    , mPublisher(nullptr)
    , mSubscriber(nullptr)
    , mReaderReport(nullptr)
{
    /* empty */
}

Participant::~Participant() {
    mWriters.clear();
    mReaderReport.reset();
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

void Participant::onLoop() {
    // time to print statistic
    std::stringstream ss;
    std::shared_lock lock(mProtectedWriters);
    for (auto const& [key, val]: mWriters) {
        if (val.first && val.second) {
            auto msg_sent = val.first->getCountStatus();
            auto msg_gen = val.second->getStatusGen();
            if (msg_sent.first && msg_gen.first) {
                ss << key << ": Sent: " << msg_sent.second << "; Lost: " << msg_gen.second - msg_sent.second  << std::endl;
            }
        }
    }
    lock.unlock();
    auto str = ss.str();
    if (!str.empty()) {
        ss << std::endl;
        mSignals->sigPrintLogText(ss.str());
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

    mReaderReport = std::make_shared<ReaderReport>("Report", "report", mSignals);
    mReaderReport->init(mDomainParticipant, mSubscriber);

    initThread();
    return true;
}

bool Participant::createWriter(const std::string &topicName, const std::string &typeName, size_t sizePayload, uint32_t timeToSend, uint32_t timeToGen) {
    bool result = false;
    {
        std::unique_lock lock(mProtectedWriters);
        if (mWriters.find(topicName) != mWriters.end()) {
            return result;
        }
        mWriters[topicName] = {nullptr, nullptr};
    }

    if (!AvailableDataTypes::instance().isType(typeName)) {
        return result;
    }

    if (typeName == "msg1") {
        result = createWriterMsg<Message_1>(topicName, typeName, sizePayload, timeToSend, timeToGen, &genMsg1);
    } else if (typeName == "msg2") {
        result = createWriterMsg<Message_2>(topicName, typeName, sizePayload, timeToSend, timeToGen, &genMsg2);
    }

    return result;
}

void Participant::destroyWriter(const std::string &topicName) {
    std::unique_lock lock(mProtectedWriters);
    mWriters.erase(topicName);
}

template<typename DataType>
bool Participant::createWriterMsg(
        const std::string &topicName,
        const std::string &typeName,
        size_t sizePayload,
        uint32_t timeToSend,
        uint32_t timeToGen,
        FuncGen<DataType> fun)
{
    bool result = false;
    auto writer = std::make_shared<WriterImpl<DataType>>(topicName, typeName, timeToSend);
    auto generator = std::make_shared<DataSourceImpl<DataType>>(topicName, timeToGen, sizePayload);
    auto g_init = generator->init(fun);
    auto w_init = writer->init(generator, mDomainParticipant, mPublisher);

    std::unique_lock lock(mProtectedWriters);

    if (g_init && w_init) {
        mWriters[topicName] = {writer, generator};
        result = true;
    } else {
        mWriters.erase(topicName);
    }
    return result;
}
