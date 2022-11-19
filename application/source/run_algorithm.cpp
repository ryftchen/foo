#include "run_algorithm.hpp"
#include <variant>
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

#define ALGORITHM_PRINT_TASK_BEGIN_TITLE(taskType)                                                                 \
    std::cout << "\r\n"                                                                                            \
              << "ALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
#define ALGORITHM_PRINT_TASK_END_TITLE(taskType)                                                                   \
    }                                                                                                              \
    std::cout << "\r\n"                                                                                            \
              << "ALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << "\r\n"       \
              << std::endl;

namespace run_algo
{
using util_hash::operator""_bkdrHash;
using Type = AlgorithmTask::Type;
template <class T>
using Bottom = AlgorithmTask::Bottom<T>;
using MatchMethod = AlgorithmTask::MatchMethod;
using NotationMethod = AlgorithmTask::NotationMethod;
using OptimalMethod = AlgorithmTask::OptimalMethod;
using SearchMethod = AlgorithmTask::SearchMethod;
using SortMethod = AlgorithmTask::SortMethod;

AlgorithmTask& getTask()
{
    static AlgorithmTask task;
    return task;
}

namespace input
{
constexpr std::string_view singlePatternForMatch{"12345"};
constexpr std::string_view infixForNotation{"a+b*(c^d-e)^(f+g*h)-i"};
constexpr double arrayRangeForSearch1 = -50.0;
constexpr double arrayRangeForSearch2 = 150.0;
constexpr uint32_t arrayLengthForSearch = 53;
constexpr int arrayRangeForSort1 = -50;
constexpr int arrayRangeForSort2 = 150;
constexpr uint32_t arrayLengthForSort = 53;

class Griewank : public algo_optimal::function::Function
{
public:
    double operator()(const double x) const override
    {
        // f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
        return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x));
    }

    static constexpr double range1{-600.0};
    static constexpr double range2{600.0};
    static constexpr std::string_view funcStr{
        "f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600] (one-dimensional Griewank)"};
};

class Rastrigin : public algo_optimal::function::Function
{
public:
    double operator()(const double x) const override
    {
        // f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
        return (x * x - 10.0 * std::cos(2.0 * M_PI * x) + 10.0);
    }

    static constexpr double range1{-5.12};
    static constexpr double range2{5.12};
    static constexpr std::string_view funcStr{
        "f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12] (one-dimensional Rastrigin)"};
};
} // namespace input

void runMatch(const std::vector<std::string>& targets)
{
    if (getBit<MatchMethod>().none())
    {
        return;
    }

    using input::singlePatternForMatch;
    static_assert(algo_match::maxDigit > singlePatternForMatch.length());
    ALGORITHM_PRINT_TASK_BEGIN_TITLE(Type::match);

    using algo_match::MatchSolution;
    const std::shared_ptr<MatchSolution> match = std::make_shared<MatchSolution>(algo_match::maxDigit);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<MatchMethod>().count()), static_cast<uint32_t>(Bottom<MatchMethod>::value)));
    const auto matchFunctor =
        [&](const std::string& threadName,
            int (MatchSolution::*methodPtr)(const char*, const char*, const uint32_t, const uint32_t) const)
    {
        threads->enqueue(
            threadName,
            methodPtr,
            match,
            match->getMatchingText().get(),
            singlePatternForMatch.data(),
            match->getLength(),
            singlePatternForMatch.length());
    };

    for (int i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!getBit<MatchMethod>().test(MatchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "m_" + targetMethod;
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    ALGORITHM_PRINT_TASK_END_TITLE(Type::match);
}

void updateMatchTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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

void runNotation(const std::vector<std::string>& targets)
{
    if (getBit<NotationMethod>().none())
    {
        return;
    }

    ALGORITHM_PRINT_TASK_BEGIN_TITLE(Type::notation);

    using algo_notation::NotationSolution;
    using input::infixForNotation;
    const std::shared_ptr<NotationSolution> notation = std::make_shared<NotationSolution>(infixForNotation);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<NotationMethod>().count()), static_cast<uint32_t>(Bottom<NotationMethod>::value)));
    const auto notationFunctor =
        [&](const std::string& threadName, std::string (NotationSolution::*methodPtr)(const std::string&) const)
    {
        threads->enqueue(threadName, methodPtr, notation, std::string{infixForNotation});
    };

    for (int i = 0; i < Bottom<NotationMethod>::value; ++i)
    {
        if (!getBit<NotationMethod>().test(NotationMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "n_" + targetMethod;
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    ALGORITHM_PRINT_TASK_END_TITLE(Type::notation);
}

void updateNotationTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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

void runOptimal(const std::vector<std::string>& targets)
{
    if (getBit<OptimalMethod>().none())
    {
        return;
    }

    using Griewank = input::Griewank;
    using Rastrigin = input::Rastrigin;
    typedef std::variant<Griewank, Rastrigin> OptimalFuncTarget;
    const auto printFunctor = [](const OptimalFuncTarget& function)
    {
        constexpr std::string_view prefix{"\r\nOptimal function: "};
        std::visit(
            algo_optimal::function::FuncOverloaded{
                [&prefix](const Griewank& /*unused*/)
                {
                    std::cout << prefix << Griewank::funcStr << std::endl;
                },
                [&prefix](const Rastrigin& /*unused*/)
                {
                    std::cout << prefix << Rastrigin::funcStr << std::endl;
                },
            },
            function);
    };
    const auto resultFunctor = [targets](
                                   const algo_optimal::function::Function& function,
                                   const algo_optimal::function::FuncRange<double, double>& range)
    {
        assert((range.range1 < range.range2) && (algo_optimal::epsilon > 0.0));
        std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
            static_cast<uint32_t>(getBit<OptimalMethod>().count()),
            static_cast<uint32_t>(Bottom<OptimalMethod>::value)));
        const auto optimalFunctor =
            [&](const std::string& threadName, const std::shared_ptr<algo_optimal::OptimalSolution>& classPtr)
        {
            threads->enqueue(
                threadName,
                &algo_optimal::OptimalSolution::operator(),
                classPtr,
                range.range1,
                range.range2,
                algo_optimal::epsilon);
        };

        for (int i = 0; i < Bottom<OptimalMethod>::value; ++i)
        {
            if (!getBit<OptimalMethod>().test(OptimalMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "o_" + targetMethod;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "gra"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algo_optimal::Gradient>(function));
                    break;
                case "ann"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algo_optimal::Annealing>(function));
                    break;
                case "par"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algo_optimal::Particle>(function));
                    break;
                case "gen"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<algo_optimal::Genetic>(function));
                    break;
                default:
                    LOG_DBG("Unable to execute unknown optimal method.");
                    break;
            }
        }
    };

    ALGORITHM_PRINT_TASK_BEGIN_TITLE(Type::optimal);

    const std::unordered_multimap<
        algo_optimal::function::FuncRange<double, double>,
        OptimalFuncTarget,
        algo_optimal::function::FuncMapHash>
        optimalFuncMap{
            {{Griewank::range1, Griewank::range2, Griewank::funcStr}, Griewank()},
            {{Rastrigin::range1, Rastrigin::range2, Rastrigin::funcStr}, Rastrigin()}};
    for ([[maybe_unused]] const auto& [range, expression] : optimalFuncMap)
    {
        printFunctor(expression);
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

    ALGORITHM_PRINT_TASK_END_TITLE(Type::optimal);
}

void updateOptimalTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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

void runSearch(const std::vector<std::string>& targets)
{
    if (getBit<SearchMethod>().none())
    {
        return;
    }

    using input::arrayLengthForSearch;
    using input::arrayRangeForSearch1;
    using input::arrayRangeForSearch2;
    static_assert((arrayRangeForSearch1 < arrayRangeForSearch2) && (arrayLengthForSearch > 0));
    ALGORITHM_PRINT_TASK_BEGIN_TITLE(Type::search);

    using algo_search::SearchSolution;
    const std::shared_ptr<SearchSolution<double>> search =
        std::make_shared<SearchSolution<double>>(arrayLengthForSearch, arrayRangeForSearch1, arrayRangeForSearch2);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<SearchMethod>().count()), static_cast<uint32_t>(Bottom<SearchMethod>::value)));
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
            search->getSearchedKey());
    };

    for (int i = 0; i < Bottom<SearchMethod>::value; ++i)
    {
        if (!getBit<SearchMethod>().test(SearchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "s_" + targetMethod;
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    ALGORITHM_PRINT_TASK_END_TITLE(Type::search);
}

void updateSearchTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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

void runSort(const std::vector<std::string>& targets)
{
    if (getBit<SortMethod>().none())
    {
        return;
    }

    using input::arrayLengthForSort;
    using input::arrayRangeForSort1;
    using input::arrayRangeForSort2;
    static_assert((arrayRangeForSort1 < arrayRangeForSort2) && (arrayLengthForSort > 0));
    ALGORITHM_PRINT_TASK_BEGIN_TITLE(Type::sort);

    using algo_sort::SortSolution;
    const std::shared_ptr<SortSolution<int>> sort =
        std::make_shared<SortSolution<int>>(arrayLengthForSort, arrayRangeForSort1, arrayRangeForSort2);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<SortMethod>().count()), static_cast<uint32_t>(Bottom<SortMethod>::value)));
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
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    ALGORITHM_PRINT_TASK_END_TITLE(Type::sort);
}

void updateSortTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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
} // namespace run_algo
