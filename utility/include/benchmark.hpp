//! @file benchmark.hpp
//! @author ryftchen
//! @brief The declarations (benchmark) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Benchmark-related functions in the utility module.
namespace benchmark
{
extern const char* version() noexcept;

//! @brief Escape the pointer to prevent optimization. Used for benchmark.
//! @param p - pointer to escape
inline void escape(const void* const p)
{
    asm volatile("" : : "g"(p) : "memory"); // NOLINT(hicpp-no-assembler)
}

//! @brief Clobber the memory to prevent optimization. Used for benchmark.
inline void clobber()
{
    asm volatile("" : : : "memory"); // NOLINT(hicpp-no-assembler)
}
} // namespace benchmark
} // namespace utility
