#pragma once

template<typename DataType>
Writer<DataType>::Writer(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
    : ThreadBase(topicName + "_wrt", timeOutToSend)
    , mTypeName(typeName)
    , mTopicName(topicName)
    , mDataSource()
    , mDomainParticipant(nullptr)
    , mPublisher(nullptr)
    , mDataWriter(nullptr)
    , mTopic(nullptr)
    , mCountSent(0)
{
    /* empty */
}

template<typename DataType>
Writer<DataType>::~Writer() {
    closeThread();
    if (mDataWriter) {
        mPublisher->delete_datawriter(mDataWriter);
        mDataWriter = nullptr;
    }
    if (mTopic) {
        mDomainParticipant->delete_topic(mTopic);
        mTopic = nullptr;
    }
}

template<typename DataType>
bool Writer<DataType>::init(std::weak_ptr<IDataSource<DataType>> dataSource,
                            eprosima::fastdds::dds::DomainParticipant *domainParticipant,
                            eprosima::fastdds::dds::Publisher *publisher) {
    using namespace eprosima::fastdds::dds;
    mDataSource = std::move(dataSource);
    mDomainParticipant = domainParticipant;
    mPublisher = publisher;

    if (!mDomainParticipant || !mPublisher || mDataSource.expired()) {
        return false;
    }

    mTopic = mDomainParticipant->create_topic(mTopicName, mTypeName, TOPIC_QOS_DEFAULT);
    if (!mTopic) {
        return false;
    }

    mDataWriter = mPublisher->create_datawriter(mTopic, DATAWRITER_QOS_DEFAULT, nullptr);
    if (!mDataWriter) {
        mDomainParticipant->delete_topic(mTopic);
        mTopic = nullptr;
        return false;
    }

    if (!initThread()) {
        mPublisher->delete_datawriter(mDataWriter);
        mDomainParticipant->delete_topic(mTopic);
        mDataWriter = nullptr;
        mTopic = nullptr;
    }

    return true;
}

template<typename DataType>
size_t Writer<DataType>::getNumberMessagesSent() const {
    return mCountSent;
}

template<typename DataType>
void Writer<DataType>::onLoop() {
    std::cout << "From Writer: time to send\n";
    if (auto source = mDataSource.lock(); source) {
        auto data = source->popData();
        if (data && mDataWriter->write(data.get())) {
            mCountSent++;
        }
    }
}

