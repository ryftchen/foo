#pragma once

#include <bitset>
#include <sstream>
#include <vector>

namespace app_algo
{
class AlgorithmTask
{
public:
    template <class T>
    struct Bottom;

    enum Type
    {
        match,
        notation,
        optimal,
        search,
        sort
    };

    enum MatchMethod
    {
        rabinKarp,
        knuthMorrisPratt,
        boyerMoore,
        horspool,
        sunday
    };
    template <>
    struct Bottom<MatchMethod>
    {
        static constexpr int value = 5;
    };

    enum NotationMethod
    {
        prefix,
        postfix
    };
    template <>
    struct Bottom<NotationMethod>
    {
        static constexpr int value = 2;
    };

    enum OptimalMethod
    {
        gradient,
        annealing,
        particle,
        genetic
    };
    template <>
    struct Bottom<OptimalMethod>
    {
        static constexpr int value = 4;
    };

    enum SearchMethod
    {
        binary,
        interpolation,
        fibonacci
    };
    template <>
    struct Bottom<SearchMethod>
    {
        static constexpr int value = 3;
    };

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
    template <>
    struct Bottom<SortMethod>
    {
        static constexpr int value = 10;
    };

    std::bitset<Bottom<MatchMethod>::value> matchBit;
    std::bitset<Bottom<NotationMethod>::value> notationBit;
    std::bitset<Bottom<OptimalMethod>::value> optimalBit;
    std::bitset<Bottom<SearchMethod>::value> searchBit;
    std::bitset<Bottom<SortMethod>::value> sortBit;

    [[nodiscard]] inline bool empty() const
    {
        return (matchBit.none() && notationBit.none() && optimalBit.none() && searchBit.none() && sortBit.none());
    }
    inline void reset()
    {
        matchBit.reset();
        notationBit.reset();
        optimalBit.reset();
        searchBit.reset();
        sortBit.reset();
    }

protected:
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
} // namespace app_algo
