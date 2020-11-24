#include <stdexcept>
#include <iostream>
#include <csignal>
#include <ParserArgs.h>
#include "Application.h"
#include "DataGeneratorFactory.h"

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
{
    if (mInstance) {
        throw std::runtime_error("An instance of the Application class has already been created.");
    }
    mInstance = this;
}

Application::~Application() {
    mInstance = nullptr;
}

void Application::sigStopApp(int num) {
    Signal signal([&]() {
        mIsRun = false;
    });
    std::lock_guard lock(mProtectSignals);
    if (num == SIGTERM || num == SIGINT) {
        mIsRun = false;
    }
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
}

void Application::sigPrintLogText(const std::string &message) {
    Signal signal([message](){
        std::cout << message << std::endl;
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
}

void Application::sigAddSender(const DConfigSender &config) {
    Signal signal([](){
        std::cout << "sigAddSender" << std::endl;
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
}

void Application::sigAddReceiver(const DConfigReceiver &config) {
    Signal signal([](){
        std::cout << "sigAddReceiver" << std::endl;
    });
    std::lock_guard lock(mProtectSignals);
    mSignals.push_back(std::move(signal));
    mWaitSignal.notify_all();
}

bool Application::init() {
    ParserOpt opt(mArgc, mArgv);
    ParserOpt::ParserElement<std::string> conf_file('c', "config", "configuration files", "FILE",false);
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

    return true;
}

int Application::main() {
    if (!mIsRun) {
        return 0;
    }
    if (mSignals.empty()) {
        mSignals.emplace_back([](){}); // fake signal
    }
    std::cout << "Starting the main event loop.\n";
    do {
        std::unique_lock lock(mProtectSignals);
        mSignals.front()();
        mSignals.pop_front();
        mWaitSignal.wait(lock, [this]() {
            return !mSignals.empty();
        });
    } while (mIsRun);
    DataGeneratorFactory::instance()->destroyAllGenerators();
    std::cout << "The main event loop is stopped.\n";
    return 0;
}
