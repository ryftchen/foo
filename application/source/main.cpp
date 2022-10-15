#include "main.hpp"
#include "command.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

int main(int argc, char* argv[])
{
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(2);
    threads->enqueue("logger", &util_log::Log::runLogger, &util_log::logger);
    threads->enqueue("commander", &Command::runCommander, std::make_shared<Command>(), argc, argv);

    return 0;
}
