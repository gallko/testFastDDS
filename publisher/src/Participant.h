#pragma once
#include <shared_mutex>
#include <ThreadBase.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>

class ISignals;
class IWriter;
class DataGenerator;
class DWriterConfig;

class Participant : public utils::ThreadBase, public eprosima::fastdds::dds::DataReaderListener {
public:
    Participant(const std::string& name, uint32_t timeOutLog, std::shared_ptr<ISignals> signals);
    ~Participant() override;
    bool init();

    bool createWriter(const std::string &topicName, const std::string &dataName, size_t sizePayload, uint32_t timeToSend, uint32_t timeToGen);
    void destroyWriter(const std::string &topicName);

private:
    void on_data_available(eprosima::fastdds::dds::DataReader *reader) override;

private:
    using WriterGen = std::pair<std::shared_ptr<IWriter>, std::shared_ptr<DataGenerator>>;
    using Container = std::map<std::string, WriterGen>;

    void onLoop() override;
    void clearDDS();

    std::shared_mutex mProtectedContainer;
    Container mWriters;

    const std::string mName;
    std::shared_ptr<ISignals> mSignals;

    eprosima::fastdds::dds::DomainParticipant *mDomainParticipant;
    eprosima::fastdds::dds::Publisher *mPublisher;

    eprosima::fastdds::dds::Subscriber* mSubscriber;
    eprosima::fastdds::dds::Topic *mTopicReport;
    eprosima::fastdds::dds::DataReader *mReaderReport;
};
