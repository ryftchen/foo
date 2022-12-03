#include "main.hpp"
#include "command.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

int main(int argc, char* argv[])
{
    using application::command::Command;
    using utility::log::Log;

    std::shared_ptr<utility::thread::Thread> thread = std::make_shared<utility::thread::Thread>(2);
    thread->enqueue("commander", &Command::runCommander, &Command::getInstance(), argc, argv);
    thread->enqueue("logger", &Log::runLogger, &Log::getInstance());

    return 0;
}
