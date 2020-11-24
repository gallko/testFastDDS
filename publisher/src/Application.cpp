#include <cassert>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <IDataGeneratorFactory.h>
#include "Application.h"

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

bool Application::init() {
    return true;
}

int Application::main() {
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
    IDataGeneratorFactory::instance()->destroyAllGenerators();
    std::cout << "The main event loop is stopped.\n";
    return 0;
}
