#include "main.hpp"
#include "command.hpp"
#include "log.hpp"
#include "thread.hpp"

int main(int argc, char* argv[])
{
    util_thread::Thread threadPool(2);
    threadPool.enqueue("log", &util_log::Log::runLogger, &util_log::logger);
    threadPool.enqueue("command", &Command::runCommander, std::make_shared<Command>(), argc, argv);

    return 0;
}
