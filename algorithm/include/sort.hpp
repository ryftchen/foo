//! @file sort.hpp
//! @author ryftchen
//! @brief The declarations (sort) in the algorithm module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Sort-related functions in the algorithm module.
namespace sort
{
//! @brief Sort methods.
//! @tparam T - type of sort methods
template <class T>
class Sort
{
public:
    //! @brief Destroy the Sort object.
    virtual ~Sort() = default;

    //! @brief Bubble.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> bubble(const T* const array, const std::uint32_t length);
    //! @brief Selection.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> selection(const T* const array, const std::uint32_t length);
    //! @brief Insertion.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> insertion(const T* const array, const std::uint32_t length);
    //! @brief Shell.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> shell(const T* const array, const std::uint32_t length);
    //! @brief Merge.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> merge(const T* const array, const std::uint32_t length);
    //! @brief Quick.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> quick(const T* const array, const std::uint32_t length);
    //! @brief Heap.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> heap(const T* const array, const std::uint32_t length);
    //! @brief Counting.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> counting(const T* const array, const std::uint32_t length);
    //! @brief Bucket.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> bucket(const T* const array, const std::uint32_t length);
    //! @brief Radix.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> radix(const T* const array, const std::uint32_t length);

private:
    //! @brief Recursive for the merge method.
    //! @param sortArray - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void mergeSortRecursive(T* const sortArray, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Recursive for the quick method.
    //! @param sortArray - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void quickSortRecursive(T* const sortArray, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Build maximum heap for the heap method.
    //! @param sortArray - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void buildMaxHeap(T* const sortArray, const std::uint32_t begin, const std::uint32_t end);
};
} // namespace sort
} // namespace algorithm

#include "algorithm/source/sort.tpp"
