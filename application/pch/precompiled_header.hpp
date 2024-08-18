//! @file precompiled_header.hpp
//! @author ryftchen
//! @brief The precompiled header in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#ifndef APPLICATION_PCH_H
#define APPLICATION_PCH_H

#include <openssl/evp.h>
#include <readline/readline.h>
#include <sys/ipc.h>
#include <sys/prctl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#if defined(__has_include) && __has_include(<gmp.h>)
#include <gmp.h>
#endif // defined(__has_include) && __has_include(<gmp.h>)
#include <mpfr.h>
#if defined(__has_include) && __has_include(<ncurses.h>)
#include <ncurses.h>
#endif // defined(__has_include) && __has_include(<ncurses.h>)
#include <algorithm>
#include <barrier>
#include <bitset>
#include <cassert>
#include <cmath>
#include <coroutine>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <latch>
#include <memory>
#include <queue>
#include <ranges>
#include <regex>
#include <span>
#include <sstream>
#include <string>
#include <syncstream>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#endif // APPLICATION_PCH_H
