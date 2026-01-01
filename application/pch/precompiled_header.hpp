//! @file precompiled_header.hpp
//! @author ryftchen
//! @brief The precompiled header in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#ifndef APPLICATION_PCH_H
#define APPLICATION_PCH_H

#include <gsl/gsl_sf.h>
#if __has_include(<gsl/gsl_version.h>)
#include <gsl/gsl_version.h>
#endif // __has_include(<gsl/gsl_version.h>)
#include <netinet/in.h>
#include <openssl/evp.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/prctl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#if __has_include(<gmp.h>)
#include <gmp.h>
#endif // __has_include(<gmp.h>)
#include <lz4.h>
#include <mpfr.h>
#if __has_include(<ncurses.h>)
#include <ncurses.h>
#endif // __has_include(<ncurses.h>)
#include <algorithm>
#include <array>
#include <barrier>
#include <bitset>
#include <cassert>
#include <coroutine>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <latch>
#include <list>
#include <memory>
#include <numeric>
#include <ranges>
#include <regex>
#include <source_location>
#include <span>
#include <sstream>
#include <string>
#include <syncstream>
#include <vector>

#endif // APPLICATION_PCH_H
