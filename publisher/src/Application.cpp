#include <stdexcept>
#include <iostream>
#include <csignal>
#include <ParserArgs.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <AvailableDataTypes.h>
#include "Application.h"
#include "Participant.h"
#include "model/DParticipantConfig.h"
#include "model/DWriterConfig.h"

namespace {
    const char FieldName[] = "name";
    const char FieldTimeToReport[] = "timeToReport";
    const char FieldTopics[] = "topics";

    const char FieldTopicName[] = "name";
    const char FieldDataType[] = "typeName";
    const char FieldSizePayload[] = "sizePayload";
    const char FieldTimeToGeneration[] = "timeToGeneration";
    const char FieldTimeToSend[] = "timeToSend";
}

Application *Application::mInstance = nullptr;

int Application::exec() {
    return mInstance ? mInstance->main() : 1;
}

Application::Application(int argc, char **argv)
    : mArgc(argc)
    , mArgv(argv)
    , mIsRun(true)
    , mProtectSignals()
    , mSignals()
    , mWaitSignal()
    , mParticipant(nullptr)
{
    if (mInstance) {
        throw std::runtime_error("An instance of the Application class has already been created.");
    }
    mInstance = this;
}

Application::~Application() {
    mInstance = nullptr;
}

std::future<void> Application::sigStopApp(int num) {
    Signal signal([this, num](int *return_code) {
        *return_code = num;
        mIsRun = false;
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
    return mSignals.back().get_future();
}

std::future<void> Application::sigCreateParticipant(const DParticipantConfig &config) {
    Signal signal([this, config](int *return_code) {
        mParticipant = std::make_shared<Participant>(config.mName, config.mTimeOutLog, std::shared_ptr<ISignals>(this, [](void *){}));
        *return_code = !mParticipant->init();
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
    return mSignals.back().get_future();
}

std::future<void> Application::sigCreateWriter(const DWriterConfig &config) {
    Signal signal([this, config](int *return_code) {
        *return_code = !mParticipant->createWriter(config.mName, config.mDataType, config.mSizePayload, config.mTimeToSend, config.mTimeToGeneration);
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
    return mSignals.back().get_future();
}

std::future<void> Application::sigPrintLogText(const std::string &message) {
    Signal signal([message](int *return_code) {
        std::cout << message;
        *return_code = 0;
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
    return mSignals.back().get_future();
}

bool Application::init() {
    ParserOpt opt(mArgc, mArgv);
    ParserOpt::ParserElement<std::string> conf_file('c', "config", "configuration files", "FILE",true);
    ParserOpt::ParserElement<bool> help('h', "help", "display this help and exit", "",false);
    opt.add_option(conf_file);
    opt.add_option(help);
    auto flag = opt.parsing();
    if (help.getValue()) {
        opt.print_help(stdout);
        mIsRun = false;
        return true;
    }
    if (!flag) {
        std::cout << opt.getError() << std::endl;
        return false;
    }
    parse_conf(conf_file.getValue());
    return true;
}

int Application::main() {
    if (!mIsRun) {
        return 0;
    }
    if (mSignals.empty()) {
        mSignals.emplace_back([](int *return_code){
            *return_code = 0;
        }); // fake signal
    }
    std::cout << "Starting the main event loop.\n";
    do {
        std::unique_lock lock(mProtectSignals);
        auto func = std::move(mSignals.front());
        mSignals.pop_front();
        lock.unlock();

        int ret_code = 0;
        func(&ret_code);

        if (ret_code == 0) {
            lock.lock();
            mWaitSignal.wait(lock, [this]() {
                return !mSignals.empty();
            });
        } else {
            mIsRun = false;
        }
    } while (mIsRun);
    std::cout << "The main event loop is stopped.\n";
    return 0;
}

void Application::parse_conf(const std::string &file) {
    using namespace nlohmann;
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
        throw std::runtime_error("can't open");
    }
    auto j = json::parse(ifs);
    if (!(j.contains(FieldName) && j[FieldName].is_string())) {
        throw std::runtime_error(std::string("missing or incorrect required \"") + FieldName +"\" field");
    }
    if (!(j.contains(FieldTimeToReport) && j[FieldTimeToReport].is_number_unsigned())) {
        throw std::runtime_error(std::string("missing or incorrect required \"") + FieldTimeToReport + "\" field");
    }
    sigCreateParticipant({j[FieldName], j[FieldTimeToReport]});


    if (!(j.contains(FieldTopics) && j[FieldTopics].is_array())) {
        throw std::runtime_error(std::string("missing or incorrect required \"") + FieldTopics + "\" field");
    }
    for (auto &topic: j[FieldTopics]) {
        auto conf = DWriterConfig(topic[FieldTopicName], topic[FieldDataType], topic[FieldSizePayload],
                                  topic[FieldTimeToGeneration],
                                  topic[FieldTimeToSend]);
        sigCreateWriter(conf);
    }
}
