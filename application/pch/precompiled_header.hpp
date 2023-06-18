//! @file precompiled_header.hpp
//! @author ryftchen
//! @brief The precompiled header in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifdef __PRECOMPILED_HEADER
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <mpfr.h>
#include <unistd.h>
#include <array>
#include <bitset>
#include <map>
// #define NDEBUG
#include <cassert>
#include <cmath>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <regex>
#include <sstream>
#include <tuple>
#include <vector>
#endif // __PRECOMPILED_HEADER
