#pragma once
#include <ThreadBase.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>

class ISignals;
class IWriter;

class Participant : public utils::ThreadBase, public eprosima::fastdds::dds::DataReaderListener {
public:
    Participant(const std::string& name, uint32_t timeOutLog, std::shared_ptr<ISignals> signals);
    ~Participant() override;
    bool init();

    bool creatWriter(const std::string &topicName);
    bool destroyWriter(const std::string &topicName);


private:
    void on_data_available(eprosima::fastdds::dds::DataReader *reader) override;

private:

    void onLoop() override;
    void clearDDS();

    std::shared_mutex mProtectedWriters;
    std::map<std::string, std::shared_ptr<IWriter>> mWriters;

    const std::string mName;
    std::shared_ptr<ISignals> mSignals;

    eprosima::fastdds::dds::DomainParticipant *mDomainParticipant;
    eprosima::fastdds::dds::Publisher *mPublisher;

    eprosima::fastdds::dds::Subscriber* mSubscriber;
    eprosima::fastdds::dds::Topic *mTopicReport;
    eprosima::fastdds::dds::DataReader *mReaderReport;

    eprosima::fastdds::dds::TypeSupport mTypeSupportMsg1;
    eprosima::fastdds::dds::TypeSupport mTypeSupportReport;
};
