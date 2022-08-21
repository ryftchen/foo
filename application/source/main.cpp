#include "main.hpp"
#include "command.hpp"
#include "log.hpp"
#include "thread.hpp"

int main(int argc, char* argv[])
{
    Thread threadPool(2);
    threadPool.enqueue("log", &Log::runLogger, &logger);
    threadPool.enqueue("command", &Command::runCommander, std::make_shared<Command>(), argc, argv);

    return 0;
}
