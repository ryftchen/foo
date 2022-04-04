#include "main.hpp"
#include "command.hpp"
#include "log.hpp"

int main(int argc, char* argv[])
{
    const std::shared_ptr<Command> cmd = std::make_shared<Command>(argc, argv);
    if (cmd->checkTask())
    {
        cmd->doTask();
    }

    return 0;
}
