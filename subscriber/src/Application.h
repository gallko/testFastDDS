#pragma once

#include <list>
#include <mutex>
#include <condition_variable>
#include <future>

#include <ISignals.h>

class Participant;

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

    std::future<void> sigPrintLogText(const std::string &message) override;
    std::future<void> sigStopApp(int num = 0) override;
    std::future<void> sigCreateParticipant(const DParticipantConfig &config) override;
    std::future<void> sigCreateWriter(const DWriterConfig &config) override;
    std::future<void> sigCreateReader(const DReaderConfig &config) override;


private:
    using Signal = std::packaged_task< void(int *) >;

    int main();
    void parse_conf(const std::string &file);

    int mArgc;
    char **mArgv;

    bool mIsRun;
    std::mutex mProtectSignals;
    std::list<Signal> mSignals;
    std::condition_variable mWaitSignal;

    std::shared_ptr<Participant> mParticipant;
};
