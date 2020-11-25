#include "ThreadBase.h"

#include <utility>
#include <iostream>

namespace utils {

ThreadBase::ThreadBase(std::string name, uint32_t timeOut)
    : mTread(nullptr)
    , mName(std::move(name))
    , mLockLoop()
    , mTimeOut(timeOut)
    , mWaitLoop()
    , mReasonWakeUp(ReasonWakeUp::fake)
{
    /* empty */
}

bool ThreadBase::initThread() {
    if (!mTread) {
        mTread = std::make_unique<std::thread>(&ThreadBase::worker, this);
        if (pthread_setname_np(mTread->native_handle(), mName.c_str())) {
            closeThread();
            return false;
        }
        return true;
    }
    return false;
}

void ThreadBase::closeThread() {
    if (mTread) {
        {
            std::lock_guard lock(mLockLoop);
            mReasonWakeUp = ReasonWakeUp::closeThread;
            mWaitLoop.notify_all();
        }
        if (mTread->joinable()) {
            mTread->join();
        }
        mTread.reset();
    }
}

void ThreadBase::worker() {
    onStart();
    while (true) {
        std::unique_lock lock(mLockLoop);
        mWaitLoop.wait_for(lock, std::chrono::milliseconds(mTimeOut), [this]() {
            switch (mReasonWakeUp) {
                case ReasonWakeUp::closeThread:
                    return true;
                case ReasonWakeUp::fake:
                    return false;
            }
            return false;
        });
        if (mReasonWakeUp == ReasonWakeUp::closeThread) {
            lock.unlock();
            onClose();
            return;
        }
        lock.unlock();
        onLoop();
    }
}

std::string ThreadBase::getNameThread() const {
    return mName;
}

ThreadBase::~ThreadBase() {
    closeThread();
}

} /* utils */