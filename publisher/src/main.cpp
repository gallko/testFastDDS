#include <csignal>
#include "Application.h"

namespace {
    std::function<void(int)> shutdown_handler;
    void signal_handler(int signal) {
        shutdown_handler(signal);
    }
} // namespace

int main(int argc, char *argv[]) {
    auto ap = Application(argc, argv);
    if (!ap.init()) {
        return -1;
    }
    shutdown_handler = [&ap](int signal) {
        ap.sigStopApp(signal);
    };
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler); // ctrl+c
    return Application::exec();
}