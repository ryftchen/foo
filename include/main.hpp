#pragma once
#include <filesystem>

[[using gnu: constructor]] static void switchToProjectPath();

static void switchToProjectPath()
{
    const std::filesystem::path absolutePath =
        std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"});
    const size_t pos = absolutePath.string().find_last_of("build");
    if (std::string::npos == pos)
    {
        printf("The build directory does not exist. Please check it.\n");
        exit(-1);
    }
    const std::filesystem::path buildPath(
        std::filesystem::path{absolutePath.string().substr(0, pos)});

    if (!buildPath.has_parent_path())
    {
        printf("The project directory does not exist. Please check it.\n");
        exit(-1);
    }
    std::filesystem::current_path(buildPath.parent_path());
}
