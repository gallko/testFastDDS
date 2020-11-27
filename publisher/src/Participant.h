#pragma once
#include <shared_mutex>
#include <ThreadBase.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>


class Message_1;
class Message_2;
class ISignals;
class IWriter;
class IReader;
class DataGenerator;
class DWriterConfig;
class ReaderReport;

class Participant : public utils::ThreadBase{
public:
    Participant(const std::string& name, uint32_t timeOutLog, std::shared_ptr<ISignals> signals);
    ~Participant() override;
    bool init();

    bool createWriter(const std::string &topicName, const std::string &typeName, size_t sizePayload, uint32_t timeToSend, uint32_t timeToGen);
    void destroyWriter(const std::string &topicName);

private:
    using WriterGen = std::pair<std::shared_ptr<IWriter>, std::shared_ptr<DataGenerator>>;
    using ContainerOfWriters = std::map<std::string, WriterGen>;
    template<typename DataType> using FuncGen = std::function<std::shared_ptr<DataType>(size_t)>;

    void onLoop() override;
    void clearDDS();

    template<typename DataType>
    bool createWriterMsg(const std::string &topicName, const std::string &typeName, size_t sizePayload, uint32_t timeToSend, uint32_t timeToGen, FuncGen<DataType> fun);

    std::shared_mutex mProtectedWriters;
    ContainerOfWriters mWriters;

    std::shared_ptr<ReaderReport> mReaderReport;

    const std::string mName;
    std::shared_ptr<ISignals> mSignals;

    eprosima::fastdds::dds::DomainParticipant *mDomainParticipant;
    eprosima::fastdds::dds::Publisher *mPublisher;
    eprosima::fastdds::dds::Subscriber* mSubscriber;
};

