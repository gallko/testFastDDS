#pragma once

#include <memory>
#include <string>
#include <IWriter.h>
#include <IDataConverter.h>
#include <IDataSource.h>
#include <ThreadBase.h>
#include <Message_1.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

template<typename DataType>
class Writer: public IWriter, public utils::ThreadBase, public IDataConverter<DataType> {
public:
    Writer(const std::string &nameType, uint32_t timeOutToSend);
    ~Writer() override;

    bool init(std::weak_ptr<IDataSource<DataType>> dataSource,
              eprosima::fastdds::dds::DomainParticipant* domainParticipant, eprosima::fastdds::dds::Publisher* mPublisher);

    size_t getNumberMessagesSent() const override;

private:
    void onLoop() override;

    const std::string mNameType;
    std::weak_ptr<IDataSource<DataType>> mDataSource;
    eprosima::fastdds::dds::DomainParticipant* mDomainParticipant;
    eprosima::fastdds::dds::Publisher* mPublisher;
    eprosima::fastdds::dds::DataWriter* mDataWriter;
    eprosima::fastdds::dds::Topic* mTopic;
    std::atomic<size_t> mCountSent;
};

#include "Writer.hpp"

