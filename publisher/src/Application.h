#pragma once

#include <list>
#include <mutex>
#include <condition_variable>
#include <future>

#include <ISignals.h>

class Application final: public ISignals {
public: // static
    static int exec();

private: // static
    static Application *mInstance;

public:
    Application(int argc, char *argv[]);
    ~Application() override;

    Application(const Application&) = delete;
    Application &operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application &operator=(Application&&) = delete;

    bool init();

    void sigPrintLogText(const std::string &message) override;
    void sigStopApp(int num = 0) override;

private:
    using Signal = std::packaged_task<void()>;

    int main();

    int mArgc;
    char **mArgv;

    bool mIsRun;
    std::mutex mProtectSignals;
    std::list<Signal> mSignals;
    std::condition_variable mWaitSignal;
};
