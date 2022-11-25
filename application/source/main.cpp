#include "main.hpp"
#include "command.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

int main(int argc, char* argv[])
{
    std::shared_ptr<util_thread::Thread> thread = std::make_shared<util_thread::Thread>(2);
    thread->enqueue("logger", &util_log::Log::runLogger, &util_log::Log::getInstance());
    thread->enqueue("commander", &app_command::Command::runCommander, &app_command::Command::getInstance(), argc, argv);

    return 0;
}
