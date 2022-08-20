#include "main.hpp"
#include "command.hpp"
#include "log.hpp"

int main(int argc, char* argv[])
{
    std::shared_ptr<std::thread> logThread =
        std::make_shared<std::thread>(&Log::runLogger, &logger);
    pthread_setname_np(logThread->native_handle(), "log");

    std::shared_ptr<std::thread> commandThread = std::make_shared<std::thread>(
        &Command::runCommand, std::make_shared<Command>(), argc, argv);
    pthread_setname_np(commandThread->native_handle(), "command");

    logThread->join();
    commandThread->join();

    return 0;
}
