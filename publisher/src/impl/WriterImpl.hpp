#pragma once

template<typename DataType>
WriterImpl<DataType>::WriterImpl(const std::string &topicName, const std::string &typeName, uint32_t timeOutToSend)
    : ThreadBase(topicName + "_wrt", timeOutToSend)
    , mTypeName(typeName)
    , mTopicName(topicName)
    , mDataSource()
    , mDomainParticipant(nullptr)
    , mPublisher(nullptr)
    , mDataWriter(nullptr)
    , mTopic(nullptr)
    , mProtectCount()
    , mIsActive(false)
    , mCountSent(0)
    , mMatched(0)
{
    /* empty */
}

template<typename DataType>
WriterImpl<DataType>::~WriterImpl() {
    closeThread();
    if (mDataWriter) {
        mPublisher->delete_datawriter(mDataWriter);
    }
    if (mTopic) {
        mDomainParticipant->delete_topic(mTopic);
    }
}

template<typename DataType>
bool WriterImpl<DataType>::init(std::weak_ptr<IDataSource<DataType>> dataSource,
                                eprosima::fastdds::dds::DomainParticipant *domainParticipant,
                                eprosima::fastdds::dds::Publisher *publisher) {
    using namespace eprosima::fastdds::dds;

    if (!domainParticipant || !publisher || dataSource.expired()) {
        return false;
    }

    mDataSource = std::move(dataSource);
    mDomainParticipant = domainParticipant;
    mPublisher = publisher;

    mTopic = mDomainParticipant->create_topic(mTopicName, mTypeName, TOPIC_QOS_DEFAULT);
    if (!mTopic) {
        return false;
    }

    mDataWriter = mPublisher->create_datawriter(mTopic, DATAWRITER_QOS_DEFAULT, this);
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
void WriterImpl<DataType>::startCount() {
    std::lock_guard lock(mProtectCount);
    mIsActive = true;
}

template<typename DataType>
void WriterImpl<DataType>::stopCount() {
    std::lock_guard lock(mProtectCount);
    mIsActive = false;
    mCountSent = 0;
}

template<typename DataType>
std::pair<bool, size_t> WriterImpl<DataType>::getCountStatus() const {
    std::shared_lock lock(mProtectCount);
    return {mIsActive, mCountSent};
}


template<typename DataType>
void WriterImpl<DataType>::onLoop() {
    std::lock_guard lock(mProtectCount);
    if (mIsActive) {
        if (auto source = mDataSource.lock(); source) {
            auto data = source->popData();
            if (data && mDataWriter->write(data.get())) {
                mCountSent++;
            }
        }
    }
}

template<typename DataType>
void WriterImpl<DataType>::on_publication_matched(eprosima::fastdds::dds::DataWriter *writer, const eprosima::fastdds::dds::PublicationMatchedStatus &info) {
    int oldMatched = mMatched;
    mMatched = info.total_count;

    if (oldMatched == 0 && mMatched > 0) {
        auto source = mDataSource.lock();
        if (source) {
            source->resetCount();
        }
        startCount();
    } else if (oldMatched > 0 && mMatched == 0) {
        stopCount();
    }

    if (info.current_count_change > 1)
    {
        std::cout << "Publisher matched." << std::endl;
    }
    else if (info.current_count_change < 0)
    {
        std::cout << "Publisher unmatched." << std::endl;
    }
}
