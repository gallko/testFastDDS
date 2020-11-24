#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace utils {

class ThreadBase {
public:
    ThreadBase(const std::string &name, uint32_t timeOut);
    virtual ~ThreadBase();
    bool initThread();
    void closeThread();
    std::string getNameThread() const;

    ThreadBase(const ThreadBase&) = delete;
    ThreadBase &operator=(const ThreadBase&) = delete;
    ThreadBase(ThreadBase&&) = delete;
    ThreadBase &operator=(ThreadBase&&) = delete;

protected:
    virtual void onStart() = 0;
    virtual void onLoop() = 0;
    virtual void onClose() = 0;

private:
    void worker();

    enum class ReasonWakeUp;

    std::unique_ptr<std::thread> mTread;
    const std::string mName;
    std::mutex mLockLoop;
    std::uint32_t mTimeOut;
    std::condition_variable mWaitLoop;
    ReasonWakeUp mReasonWakeUp;
};

enum class ThreadBase::ReasonWakeUp {
    closeThread, fake
};

} /* utils */
