#pragma once

#include <memory>
#include <string>
#include <shared_mutex>

#include <IWriter.h>
#include <IDataSource.h>
#include <ThreadBase.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

template<typename DataType>
class WriterImpl: public IWriter, public utils::ThreadBase, public eprosima::fastdds::dds::DataWriterListener {
public:
    WriterImpl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend);
    ~WriterImpl() override;

    bool init(std::weak_ptr<IDataSource<DataType>> dataSource,
              eprosima::fastdds::dds::DomainParticipant* domainParticipant,
              eprosima::fastdds::dds::Publisher* publisher);

    void startCount() override;
    void stopCount() override;
    std::pair<bool, size_t> getCountStatus() const override;



private:
    void on_publication_matched(eprosima::fastdds::dds::DataWriter *writer, const eprosima::fastdds::dds::PublicationMatchedStatus &info) override;
    void onLoop() override;

    const std::string mTypeName;
    const std::string mTopicName;
    std::weak_ptr<IDataSource<DataType>> mDataSource;
    eprosima::fastdds::dds::DomainParticipant* mDomainParticipant;
    eprosima::fastdds::dds::Publisher* mPublisher;
    eprosima::fastdds::dds::DataWriter* mDataWriter;
    eprosima::fastdds::dds::Topic* mTopic;

    mutable std::shared_mutex mProtectCount;
    bool mIsActive;
    size_t mCountSent;

    int mMatched;
};

#include "WriterImpl.hpp"

