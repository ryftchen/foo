//! @file apply_algorithm.hpp
//! @author ryftchen
//! @brief The declarations (apply_algorithm) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <bitset>
#include <sstream>
#include <vector>

//! @brief Algorithm-applying-related functions in the application module.
namespace application::app_algo
{
//! @brief Manage algorithm tasks.
class AlgorithmTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T - type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific algorithm tasks.
    enum Type
    {
        match,
        notation,
        optimal,
        search,
        sort
    };

    //! @brief Enumerate specific match methods.
    enum MatchMethod
    {
        rabinKarp,
        knuthMorrisPratt,
        boyerMoore,
        horspool,
        sunday
    };
    //! @brief Store the maximum value of the MatchMethod enum.
    template <>
    struct Bottom<MatchMethod>
    {
        static constexpr int value = 5;
    };

    //! @brief Enumerate specific notation methods.
    enum NotationMethod
    {
        prefix,
        postfix
    };
    //! @brief Store the maximum value of the NotationMethod enum.
    template <>
    struct Bottom<NotationMethod>
    {
        static constexpr int value = 2;
    };

    //! @brief Enumerate specific optimal methods.
    enum OptimalMethod
    {
        gradient,
        annealing,
        particle,
        genetic
    };
    //! @brief Store the maximum value of the OptimalMethod enum.
    template <>
    struct Bottom<OptimalMethod>
    {
        static constexpr int value = 4;
    };

    //! @brief Enumerate specific search methods.
    enum SearchMethod
    {
        binary,
        interpolation,
        fibonacci
    };
    //! @brief Store the maximum value of the SearchMethod enum.
    template <>
    struct Bottom<SearchMethod>
    {
        static constexpr int value = 3;
    };

    //! @brief Enumerate specific sort methods.
    enum SortMethod
    {
        bubble,
        selection,
        insertion,
        shell,
        merge,
        quick,
        heap,
        counting,
        bucket,
        radix
    };
    //! @brief Store the maximum value of the SortMethod enum.
    template <>
    struct Bottom<SortMethod>
    {
        static constexpr int value = 10;
    };

    //! @brief Bit flags for managing match methods.
    std::bitset<Bottom<MatchMethod>::value> matchBit;
    //! @brief Bit flags for managing notation methods.
    std::bitset<Bottom<NotationMethod>::value> notationBit;
    //! @brief Bit flags for managing optimal methods.
    std::bitset<Bottom<OptimalMethod>::value> optimalBit;
    //! @brief Bit flags for managing search methods.
    std::bitset<Bottom<SearchMethod>::value> searchBit;
    //! @brief Bit flags for managing sort methods.
    std::bitset<Bottom<SortMethod>::value> sortBit;

    //! @brief Check whether any algorithm tasks do not exist.
    //! @return any algorithm tasks do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return (matchBit.none() && notationBit.none() && optimalBit.none() && searchBit.none() && sortBit.none());
    }
    //! @brief Reset bit flags that manage algorithm tasks.
    inline void reset()
    {
        matchBit.reset();
        notationBit.reset();
        optimalBit.reset();
        searchBit.reset();
        sortBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Type enum.
    //! @param os - output stream object
    //! @param type - the specific value of Type enum
    //! @return reference of output stream object
    friend std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        switch (type)
        {
            case Type::match:
                os << "MATCH";
                break;
            case Type::notation:
                os << "NOTATION";
                break;
            case Type::optimal:
                os << "OPTIMAL";
                break;
            case Type::search:
                os << "SEARCH";
                break;
            case Type::sort:
                os << "SORT";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
        }
        return os;
    }
};

extern AlgorithmTask& getTask();

//! @brief Get the bit flags of the method in algorithm tasks.
//! @tparam T - type of the method
//! @return bit flags of the method
template <typename T>
auto getBit()
{
    if constexpr (std::is_same_v<T, AlgorithmTask::MatchMethod>)
    {
        return getTask().matchBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::NotationMethod>)
    {
        return getTask().notationBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::OptimalMethod>)
    {
        return getTask().optimalBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SearchMethod>)
    {
        return getTask().searchBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SortMethod>)
    {
        return getTask().sortBit;
    }
}

//! @brief Set the bit flags of the method in algorithm tasks
//! @tparam T - type of the method
//! @param index - method index
template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, AlgorithmTask::MatchMethod>)
    {
        getTask().matchBit.set(AlgorithmTask::MatchMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::NotationMethod>)
    {
        getTask().notationBit.set(AlgorithmTask::NotationMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::OptimalMethod>)
    {
        getTask().optimalBit.set(AlgorithmTask::OptimalMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SearchMethod>)
    {
        getTask().searchBit.set(AlgorithmTask::SearchMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SortMethod>)
    {
        getTask().sortBit.set(AlgorithmTask::SortMethod(index));
    }
}

extern void runMatch(const std::vector<std::string>& targets);
extern void updateMatchTask(const std::string& target);
extern void runNotation(const std::vector<std::string>& targets);
extern void updateNotationTask(const std::string& target);
extern void runOptimal(const std::vector<std::string>& targets);
extern void updateOptimalTask(const std::string& target);
extern void runSearch(const std::vector<std::string>& targets);
extern void updateSearchTask(const std::string& target);
extern void runSort(const std::vector<std::string>& targets);
extern void updateSortTask(const std::string& target);
} // namespace application::app_algo
