//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include "apply_algorithm.hpp"
#include <variant>
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "application/include/command.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

//! @brief Title of printing when algorithm tasks are beginning.
#define APP_ALGO_PRINT_TASK_BEGIN_TITLE(taskType)                                                                  \
    std::cout << "\r\n"                                                                                            \
              << "ALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when algorithm tasks are ending.
#define APP_ALGO_PRINT_TASK_END_TITLE(taskType)                                                                    \
    }                                                                                                              \
    std::cout << "\r\n"                                                                                            \
              << "ALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << "\r\n"       \
              << std::endl;

namespace application::app_algo
{
//! @brief Alias for Type.
using Type = AlgorithmTask::Type;
//! @brief Alias for Bottom.
//! @tparam T type of specific enum
template <class T>
using Bottom = AlgorithmTask::Bottom<T>;
//! @brief Alias for MatchMethod.
using MatchMethod = AlgorithmTask::MatchMethod;
//! @brief Alias for NotationMethod.
using NotationMethod = AlgorithmTask::NotationMethod;
//! @brief Alias for OptimalMethod.
using OptimalMethod = AlgorithmTask::OptimalMethod;
//! @brief Alias for SearchMethod.
using SearchMethod = AlgorithmTask::SearchMethod;
//! @brief Alias for SortMethod.
using SortMethod = AlgorithmTask::SortMethod;

//! @brief Get the algorithm task.
//! @return reference of AlgorithmTask object
AlgorithmTask& getTask()
{
    static AlgorithmTask task;
    return task;
}

//! @brief Set input parameters.
namespace input
{
//! @brief Single pattern for match methods.
constexpr std::string_view singlePatternForMatch{"12345"};
//! @brief Infix for notation methods.
constexpr std::string_view infixForNotation{"a+b*(c^d-e)^(f+g*h)-i"};
//! @brief Minimum of the array for search methods.
constexpr double arrayRangeForSearch1 = -50.0;
//! @brief Maximum of the array for search methods.
constexpr double arrayRangeForSearch2 = 150.0;
//! @brief Length of the array for search methods.
constexpr uint32_t arrayLengthForSearch = 53;
//! @brief Minimum of the array for sort methods.
constexpr int arrayRangeForSort1 = -50;
//! @brief Maximum of the array for sort methods.
constexpr int arrayRangeForSort2 = 150;
//! @brief Length of the array for sort methods.
constexpr uint32_t arrayLengthForSort = 53;

//! @brief Griewank function.
class Griewank : public algorithm::optimal::function::Function
{
public:
    //! @brief The operator (()) overloading of Griewank class.
    //! @param x independent variable
    //! @return dependent variable
    double operator()(const double x) const override
    {
        // f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
        return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x));
    }

    //! @brief Left endpoint.
    static constexpr double range1{-600.0};
    //! @brief Right endpoint.
    static constexpr double range2{600.0};
    //! @brief One-dimensional Griewank.
    static constexpr std::string_view funcDescr{
        "f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600] (one-dimensional Griewank)"};
};

//! @brief Rastrigin function.
class Rastrigin : public algorithm::optimal::function::Function
{
public:
    //! @brief The operator (()) overloading of Rastrigin class.
    //! @param x independent variable
    //! @return dependent variable
    double operator()(const double x) const override
    {
        // f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
        return (x * x - 10.0 * std::cos(2.0 * M_PI * x) + 10.0);
    }

    //! @brief Left endpoint.
    static constexpr double range1{-5.12};
    //! @brief Right endpoint.
    static constexpr double range2{5.12};
    //! @brief One-dimensional Rastrigin.
    static constexpr std::string_view funcDescr{
        "f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12] (one-dimensional Rastrigin)"};
};
} // namespace input

//! @brief Run match tasks.
//! @param targets vector of target methods
void runMatch(const std::vector<std::string>& targets)
{
    if (getBit<MatchMethod>().none())
    {
        return;
    }

    using algorithm::match::MatchSolution;
    using utility::hash::operator""_bkdrHash;

    static_assert(algorithm::match::maxDigit > input::singlePatternForMatch.length());
    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Type::match);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<MatchMethod>().count()), static_cast<uint32_t>(Bottom<MatchMethod>::value)));

    const std::shared_ptr<MatchSolution> match =
        std::make_shared<MatchSolution>(algorithm::match::maxDigit, input::singlePatternForMatch);
    const auto matchFunctor =
        [&](const std::string& threadName,
            int (MatchSolution::*methodPtr)(const char*, const char*, const uint32_t, const uint32_t) const)
    {
        threads->enqueue(
            threadName,
            methodPtr,
            match,
            match->getMatchingText().get(),
            match->getSinglePattern().data(),
            std::string_view(match->getMatchingText().get()).length(),
            match->getSinglePattern().length());
    };

    for (int i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!getBit<MatchMethod>().test(MatchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "m_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
        {
            case "rab"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::rkMethod);
                break;
            case "knu"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::kmpMethod);
                break;
            case "boy"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::bmMethod);
                break;
            case "hor"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::horspoolMethod);
                break;
            case "sun"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::sundayMethod);
                break;
            default:
                LOG_DBG("execute to run unknown match method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Type::match);
}

//! @brief Update match methods in tasks.
//! @param target target method
void updateMatchTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "rab"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::rabinKarp);
            break;
        case "knu"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::knuthMorrisPratt);
            break;
        case "boy"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::boyerMoore);
            break;
        case "hor"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::horspool);
            break;
        case "sun"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::sunday);
            break;
        default:
            getBit<MatchMethod>().reset();
            throw std::runtime_error("Unexpected task of match: " + target);
    }
}

//! @brief Run notation tasks.
//! @param targets vector of target methods
void runNotation(const std::vector<std::string>& targets)
{
    if (getBit<NotationMethod>().none())
    {
        return;
    }

    using algorithm::notation::NotationSolution;
    using utility::hash::operator""_bkdrHash;

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Type::notation);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<NotationMethod>().count()), static_cast<uint32_t>(Bottom<NotationMethod>::value)));

    const std::shared_ptr<NotationSolution> notation = std::make_shared<NotationSolution>(input::infixForNotation);
    const auto notationFunctor =
        [&](const std::string& threadName, std::string (NotationSolution::*methodPtr)(const std::string&) const)
    {
        threads->enqueue(threadName, methodPtr, notation, std::string{notation->getInfixNotation()});
    };

    for (int i = 0; i < Bottom<NotationMethod>::value; ++i)
    {
        if (!getBit<NotationMethod>().test(NotationMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "n_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
        {
            case "pre"_bkdrHash:
                notationFunctor(threadName, &NotationSolution::prefixMethod);
                break;
            case "pos"_bkdrHash:
                notationFunctor(threadName, &NotationSolution::postfixMethod);
                break;
            default:
                LOG_DBG("execute to run unknown notation method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Type::notation);
}

//! @brief Update notation methods in tasks.
//! @param target target method
void updateNotationTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "pre"_bkdrHash:
            setBit<NotationMethod>(NotationMethod::prefix);
            break;
        case "pos"_bkdrHash:
            setBit<NotationMethod>(NotationMethod::postfix);
            break;
        default:
            getBit<NotationMethod>().reset();
            throw std::runtime_error("Unexpected task of notation: " + target);
    }
}

//! @brief Run optimal tasks.
//! @param targets vector of target methods
void runOptimal(const std::vector<std::string>& targets)
{
    if (getBit<OptimalMethod>().none())
    {
        return;
    }

    using input::Griewank;
    using input::Rastrigin;
    typedef std::variant<Griewank, Rastrigin> OptimalFuncTarget;

#ifndef _NO_PRINT_AT_RUNTIME
    const auto printFunctor = [](const OptimalFuncTarget& function)
    {
        constexpr std::string_view prefix{"\r\nOptimal function: "};
        std::visit(
            algorithm::optimal::function::FuncOverloaded{
                [&prefix](const Griewank& /*unused*/)
                {
                    std::cout << prefix << Griewank::funcDescr << std::endl;
                },
                [&prefix](const Rastrigin& /*unused*/)
                {
                    std::cout << prefix << Rastrigin::funcDescr << std::endl;
                },
            },
            function);
    };
#endif
    const auto resultFunctor = [targets](
                                   const algorithm::optimal::function::Function& function,
                                   const algorithm::optimal::function::FuncRange<double, double>& range)
    {
        assert((range.range1 < range.range2) && (algorithm::optimal::epsilon > 0.0));
        auto* threads = command::getMemoryForMultithreading().newElement(std::min(
            static_cast<uint32_t>(getBit<OptimalMethod>().count()),
            static_cast<uint32_t>(Bottom<OptimalMethod>::value)));
        const auto optimalFunctor =
            [&](const std::string& threadName, const std::shared_ptr<algorithm::optimal::OptimalSolution>& classPtr)
        {
            threads->enqueue(
                threadName,
                &algorithm::optimal::OptimalSolution::operator(),
                classPtr,
                range.range1,
                range.range2,
                algorithm::optimal::epsilon);
        };

        using utility::hash::operator""_bkdrHash;
        for (int i = 0; i < Bottom<OptimalMethod>::value; ++i)
        {
            if (!getBit<OptimalMethod>().test(OptimalMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "o_" + targetMethod;
            switch (utility::hash::bkdrHash(targetMethod.data()))
            {
                case "gra"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algorithm::optimal::Gradient>(function));
                    break;
                case "ann"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algorithm::optimal::Annealing>(function));
                    break;
                case "par"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algorithm::optimal::Particle>(function));
                    break;
                case "gen"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algorithm::optimal::Genetic>(function));
                    break;
                default:
                    LOG_DBG("Unable to execute unknown optimal method.");
                    break;
            }
        }
        command::getMemoryForMultithreading().deleteElement(threads);
    };

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Type::optimal);

    const std::unordered_multimap<
        algorithm::optimal::function::FuncRange<double, double>,
        OptimalFuncTarget,
        algorithm::optimal::function::FuncMapHash>
        optimalFuncMap{
            {{Griewank::range1, Griewank::range2, Griewank::funcDescr}, Griewank()},
            {{Rastrigin::range1, Rastrigin::range2, Rastrigin::funcDescr}, Rastrigin()}};
    for ([[maybe_unused]] const auto& [range, expression] : optimalFuncMap)
    {
#ifndef _NO_PRINT_AT_RUNTIME
        printFunctor(expression);
#endif
        switch (expression.index())
        {
            case 0:
                resultFunctor(std::get<0>(expression), range);
                break;
            case 1:
                resultFunctor(std::get<1>(expression), range);
                break;
                [[unlikely]] default : break;
        }
    }

    APP_ALGO_PRINT_TASK_END_TITLE(Type::optimal);
}

//! @brief Update optimal methods in tasks.
//! @param target target method
void updateOptimalTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "gra"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::gradient);
            break;
        case "ann"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::annealing);
            break;
        case "par"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::particle);
            break;
        case "gen"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::genetic);
            break;
        default:
            getBit<OptimalMethod>().reset();
            throw std::runtime_error("Unexpected task of optimal: " + target);
    }
}

//! @brief Run search tasks.
//! @param targets vector of target methods
void runSearch(const std::vector<std::string>& targets)
{
    if (getBit<SearchMethod>().none())
    {
        return;
    }

    using algorithm::search::SearchSolution;
    using input::arrayLengthForSearch;
    using input::arrayRangeForSearch1;
    using input::arrayRangeForSearch2;
    using utility::hash::operator""_bkdrHash;

    static_assert((arrayRangeForSearch1 < arrayRangeForSearch2) && (arrayLengthForSearch > 0));
    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Type::search);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<SearchMethod>().count()), static_cast<uint32_t>(Bottom<SearchMethod>::value)));

    const std::shared_ptr<SearchSolution<double>> search =
        std::make_shared<SearchSolution<double>>(arrayLengthForSearch, arrayRangeForSearch1, arrayRangeForSearch2);
    const auto searchFunctor =
        [&](const std::string& threadName,
            int (SearchSolution<double>::*methodPtr)(const double* const, const uint32_t, const double) const)
    {
        threads->enqueue(
            threadName,
            methodPtr,
            search,
            search->getOrderedArray().get(),
            search->getLength(),
            search->getSearchKey());
    };

    for (int i = 0; i < Bottom<SearchMethod>::value; ++i)
    {
        if (!getBit<SearchMethod>().test(SearchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "s_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
        {
            case "bin"_bkdrHash:
                searchFunctor(threadName, &SearchSolution<double>::binaryMethod);
                break;
            case "int"_bkdrHash:
                searchFunctor(threadName, &SearchSolution<double>::interpolationMethod);
                break;
            case "fib"_bkdrHash:
                searchFunctor(threadName, &SearchSolution<double>::fibonacciMethod);
                break;
            default:
                LOG_DBG("execute to run unknown search method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Type::search);
}

//! @brief Update search methods in tasks.
//! @param target target method
void updateSearchTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "bin"_bkdrHash:
            setBit<SearchMethod>(SearchMethod::binary);
            break;
        case "int"_bkdrHash:
            setBit<SearchMethod>(SearchMethod::interpolation);
            break;
        case "fib"_bkdrHash:
            setBit<SearchMethod>(SearchMethod::fibonacci);
            break;
        default:
            getBit<SearchMethod>().reset();
            throw std::runtime_error("Unexpected task of search: " + target);
    }
}

//! @brief Run sort tasks.
//! @param targets vector of target methods
void runSort(const std::vector<std::string>& targets)
{
    if (getBit<SortMethod>().none())
    {
        return;
    }

    using algorithm::sort::SortSolution;
    using input::arrayLengthForSort;
    using input::arrayRangeForSort1;
    using input::arrayRangeForSort2;
    using utility::hash::operator""_bkdrHash;

    static_assert((arrayRangeForSort1 < arrayRangeForSort2) && (arrayLengthForSort > 0));
    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Type::sort);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<SortMethod>().count()), static_cast<uint32_t>(Bottom<SortMethod>::value)));

    const std::shared_ptr<SortSolution<int>> sort =
        std::make_shared<SortSolution<int>>(arrayLengthForSort, arrayRangeForSort1, arrayRangeForSort2);
    const auto sortFunctor = [&](const std::string& threadName,
                                 std::vector<int> (SortSolution<int>::*methodPtr)(int* const, const uint32_t) const)
    {
        threads->enqueue(threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!getBit<SortMethod>().test(SortMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "s_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
        {
            case "bub"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::bubbleMethod);
                break;
            case "sel"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::selectionMethod);
                break;
            case "ins"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::insertionMethod);
                break;
            case "she"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::shellMethod);
                break;
            case "mer"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::mergeMethod);
                break;
            case "qui"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::quickMethod);
                break;
            case "hea"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::heapMethod);
                break;
            case "cou"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::countingMethod);
                break;
            case "buc"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::bucketMethod);
                break;
            case "rad"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::radixMethod);
                break;
            default:
                LOG_DBG("execute to run unknown sort method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Type::sort);
}

//! @brief Update sort methods in tasks.
//! @param target target method
void updateSortTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "bub"_bkdrHash:
            setBit<SortMethod>(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            setBit<SortMethod>(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            setBit<SortMethod>(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            setBit<SortMethod>(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            setBit<SortMethod>(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            setBit<SortMethod>(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            setBit<SortMethod>(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            setBit<SortMethod>(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            setBit<SortMethod>(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            setBit<SortMethod>(SortMethod::radix);
            break;
        default:
            getBit<SortMethod>().reset();
            throw std::runtime_error("Unexpected task of sort: " + target);
    }
}
} // namespace application::app_algo
