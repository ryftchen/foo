#include "../include/main.hpp"
#include <iostream>
#include "../include/command.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    switchToProjectPath();

    const std::shared_ptr<Command> cmd = std::make_shared<Command>();
    if (cmd->parseArgv(argc - 1, argv + 1))
    {
        cmd->doTask();
    }

    return 0;
}
