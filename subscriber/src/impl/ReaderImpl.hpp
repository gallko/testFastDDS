#pragma once

template<typename DataType>
ReaderImpl<DataType>::ReaderImpl(std::function<void(const DataType &)> worker, const std::string &topicName,
                                 const std::string &typeName, eprosima::fastdds::dds::DomainParticipant *participant,
                                 eprosima::fastdds::dds::Subscriber *subscriber)
    : mWorker(worker)
    , mTopicName(topicName)
    , mTypeName(typeName)
    , mDomainParticipant(participant)
    , mSubscriber(subscriber)
    , mDataReader(nullptr)
    , mTopic(nullptr)
    , mProtectCount()
    , mCountRecv(0)
    , mHaveConnection(false)
{
    /* empty */
}

template<typename DataType>
ReaderImpl<DataType>::~ReaderImpl() {
    if (mDataReader) {
        mSubscriber->delete_datareader(mDataReader);
    }
    if (mTopic) {
        mDomainParticipant->delete_topic(mTopic);
    }
}

template<typename DataType>
std::string ReaderImpl<DataType>::getName() const {
    return mTopicName;
}

template<typename DataType>
size_t ReaderImpl<DataType>::getCountRecv() const {
    std::shared_lock lock(mProtectCount);
    return mCountRecv;
}

template<typename DataType>
bool ReaderImpl<DataType>::init() {
    using namespace eprosima::fastdds::dds;

    if (!mDomainParticipant || !mSubscriber) {
        return false;
    }

    mTopic = mDomainParticipant->create_topic(mTopicName, mTypeName, TOPIC_QOS_DEFAULT);
    if (!mTopic) {
        return false;
    }

    DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
    rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    mDataReader = mSubscriber->create_datareader(mTopic, rqos, this);
    if (!mDataReader) {
        mDomainParticipant->delete_topic(mTopic);
        mTopic = nullptr;
        return false;
    }

    return true;
}

template<typename DataType>
bool ReaderImpl<DataType>::haveConnection() {
    std::shared_lock lock(mProtectCount);
    return mHaveConnection;
}

template<typename DataType>
void ReaderImpl<DataType>::on_data_available(eprosima::fastdds::dds::DataReader *reader) {
    using namespace eprosima::fastdds::dds;
    SampleInfo info;
    if (reader->take_next_sample(&mMsg, &info) == ReturnCode_t::RETCODE_OK)
    {
        if (info.instance_state == ALIVE)
        {
            {
                std::lock_guard lock(mProtectCount);
                mCountRecv++;
            }
            mWorker(mMsg);
        }
    }
}

template<typename DataType>
void ReaderImpl<DataType>::on_subscription_matched(eprosima::fastdds::dds::DataReader *reader, const eprosima::fastdds::dds::SubscriptionMatchedStatus &info) {
    std::unique_lock lock(mProtectCount);
    if (mMatched == 0 && info.total_count > 0) {
        mHaveConnection = true;
    } else if (mMatched > 0 && info.total_count == 0) {
        mCountRecv = 0;
        mHaveConnection = false;
    }
    mMatched = info.total_count;

    if (info.current_count_change > 0)
    {
        std::cout << "Publisher matched." << std::endl;
    }
    else if (info.current_count_change < 0)
    {
        std::cout << "Publisher unmatched." << std::endl;
    }
}
