#pragma once
#include <shared_mutex>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>

#include <ThreadBase.h>

class Message_1;
class Message_2;
class ISignals;
class IReader;

class Participant : public utils::ThreadBase {
public:
    Participant(const std::string& name, uint32_t timeOutReport, std::shared_ptr<ISignals> signals);
    ~Participant();
    bool init();

    bool createReader(const std::string &topicName, const std::string &typeName);
    void destroyReader(const std::string &topicName);

private:
    using Reader_ptr = std::shared_ptr<IReader>;
    using ContainerOfReaders = std::map<std::string, Reader_ptr>;

    void onLoop() override;
    void clearDDS();

    const std::string mName;
    std::shared_ptr<ISignals> mSignals;

    std::shared_mutex mProtectReaders;
    ContainerOfReaders mReaders;

    eprosima::fastdds::dds::DomainParticipant *mDomainParticipant;
    eprosima::fastdds::dds::Publisher *mPublisher;
    eprosima::fastdds::dds::Subscriber* mSubscriber;
};

