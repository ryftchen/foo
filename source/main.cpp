#include "main.hpp"
#include "command.hpp"
#include "log.hpp"

int main(int argc, char* argv[])
{
    const std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->parseArgv(argc - 1, argv + 1);
    if (cmd->checkTask())
    {
        cmd->performTask();
    }

    return 0;
}
