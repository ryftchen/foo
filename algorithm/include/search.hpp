//! @file search.hpp
//! @author ryftchen
//! @brief The declarations (search) in the algorithm module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <cstdint>
#include <vector>

//! @brief Search-related functions in the algorithm module.
namespace algorithm::search
{
//! @brief Search methods.
//! @tparam T - type of the search methods
template <class T>
class Search
{
public:
    //! @brief Destroy the Search object.
    virtual ~Search() = default;

    //! @brief Binary.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int binary(const T* const array, const std::uint32_t length, const T key);
    //! @brief Interpolation.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int interpolation(const T* const array, const std::uint32_t length, const T key);
    //! @brief Fibonacci.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int fibonacci(const T* const array, const std::uint32_t length, const T key);

private:
    //! @brief Generate Fibonacci number.
    //! @param max - the smallest integer that is not greater than the maximum value of the Fibonacci sequence
    //! @return Fibonacci sequence
    static std::vector<std::uint32_t> generateFibonacciNumber(const std::uint32_t max);
};
} // namespace algorithm::search

#include "algorithm/source/search.tpp"
