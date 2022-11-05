#pragma once

#include <bitset>
#include <condition_variable>
#include <mutex>
#include <variant>
#include "numeric/include/expression.hpp"
#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"

class Command
{
public:
    Command();
    virtual ~Command() = default;
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    void runCommander(const int argc, const char* const argv[]);

private:
    mutable std::mutex commandMutex;
    std::condition_variable commandCondition;
    std::atomic<bool> isParsed{false};
    util_argument::Argument program{util_argument::Argument("foo")};
    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle();
    void validateBasicTask();
    void validateGeneralTask();
    bool checkTask() const;
    void performTask() const;

#pragma pack(8)
    class BasicTask
    {
    public:
        template <class T>
        struct Bottom;

        enum Category
        {
            console,
            help,
            version
        };
        template <>
        struct Bottom<Category>
        {
            static constexpr int value = 3;
        };

        std::bitset<Bottom<Category>::value> primaryBit;

        [[nodiscard]] bool empty() const { return primaryBit.none(); }
        void reset() { primaryBit.reset(); }
    };

    class GeneralTask
    {
    public:
        template <class T>
        struct Bottom;

        enum Category
        {
            algorithm,
            dataStructure,
            designPattern,
            numeric
        };
        template <>
        struct Bottom<Category>
        {
            static constexpr int value = 4;
        };

        class AlgorithmTask
        {
        public:
            template <class T>
            struct Bottom;

            enum Type
            {
                match,
                notation,
                search,
                sort
            };
            template <>
            struct Bottom<Type>
            {
                static constexpr int value = 4;
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
            std::bitset<Bottom<SearchMethod>::value> searchBit;
            std::bitset<Bottom<SortMethod>::value> sortBit;

            [[nodiscard]] bool empty() const
            {
                return (matchBit.none() && notationBit.none() && searchBit.none() && sortBit.none());
            }
            void reset()
            {
                matchBit.reset();
                notationBit.reset();
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
        } algorithmTask{};

        class DataStructureTask
        {
        public:
            template <class T>
            struct Bottom;

            enum Type
            {
                linear
            };
            template <>
            struct Bottom<Type>
            {
                static constexpr int value = 1;
            };

            enum LinearInstance
            {
                linkedList,
                stack,
                queue
            };
            template <>
            struct Bottom<LinearInstance>
            {
                static constexpr int value = 3;
            };

            std::bitset<Bottom<LinearInstance>::value> linearBit;

            [[nodiscard]] bool empty() const { return (linearBit.none()); }
            void reset() { linearBit.reset(); }

        protected:
            friend std::ostream& operator<<(std::ostream& os, const Type& type)
            {
                switch (type)
                {
                    case Type::linear:
                        os << "LINEAR";
                        break;
                    default:
                        os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
                }
                return os;
            }
        } dataStructureTask{};

        class DesignPatternTask
        {
        public:
            template <class T>
            struct Bottom;

            enum Type
            {
                behavioral,
                creational,
                structural
            };
            template <>
            struct Bottom<Type>
            {
                static constexpr int value = 3;
            };

            enum BehavioralInstance
            {
                chainOfResponsibility,
                command,
                interpreter,
                iterator,
                mediator,
                memento,
                observer,
                state,
                strategy,
                templateMethod,
                visitor
            };
            template <>
            struct Bottom<BehavioralInstance>
            {
                static constexpr int value = 11;
            };

            enum CreationalInstance
            {
                abstractFactory,
                builder,
                factoryMethod,
                prototype,
                singleton
            };
            template <>
            struct Bottom<CreationalInstance>
            {
                static constexpr int value = 5;
            };

            enum StructuralInstance
            {
                adapter,
                bridge,
                composite,
                decorator,
                facade,
                flyweight,
                proxy
            };
            template <>
            struct Bottom<StructuralInstance>
            {
                static constexpr int value = 7;
            };

            std::bitset<Bottom<BehavioralInstance>::value> behavioralBit;
            std::bitset<Bottom<CreationalInstance>::value> creationalBit;
            std::bitset<Bottom<StructuralInstance>::value> structuralBit;

            [[nodiscard]] bool empty() const
            {
                return (behavioralBit.none() && creationalBit.none() && structuralBit.none());
            }
            void reset()
            {
                behavioralBit.reset();
                creationalBit.reset();
                structuralBit.reset();
            }

        protected:
            friend std::ostream& operator<<(std::ostream& os, const Type& type)
            {
                switch (type)
                {
                    case Type::behavioral:
                        os << "BEHAVIORAL";
                        break;
                    case Type::creational:
                        os << "CREATIONAL";
                        break;
                    case Type::structural:
                        os << "STRUCTURAL";
                        break;
                    default:
                        os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
                }
                return os;
            }
        } designPatternTask{};

        class NumericTask
        {
        public:
            template <class T>
            struct Bottom;

            enum Type
            {
                arithmetic,
                divisor,
                integral,
                optimal,
                prime
            };
            template <>
            struct Bottom<Type>
            {
                static constexpr int value = 5;
            };

            enum ArithmeticMethod
            {
                addition,
                subtraction,
                multiplication,
                division
            };
            template <>
            struct Bottom<ArithmeticMethod>
            {
                static constexpr int value = 4;
            };

            enum DivisorMethod
            {
                euclidean,
                stein
            };
            template <>
            struct Bottom<DivisorMethod>
            {
                static constexpr int value = 2;
            };

            enum IntegralMethod
            {
                trapezoidal,
                simpson,
                romberg,
                gauss,
                monteCarlo
            };
            template <>
            struct Bottom<IntegralMethod>
            {
                static constexpr int value = 5;
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

            enum PrimeMethod
            {
                eratosthenes,
                euler
            };
            template <>
            struct Bottom<PrimeMethod>
            {
                static constexpr int value = 2;
            };

            std::bitset<Bottom<ArithmeticMethod>::value> arithmeticBit;
            std::bitset<Bottom<DivisorMethod>::value> divisorBit;
            std::bitset<Bottom<IntegralMethod>::value> integralBit;
            std::bitset<Bottom<OptimalMethod>::value> optimalBit;
            std::bitset<Bottom<PrimeMethod>::value> primeBit;

            [[nodiscard]] bool empty() const
            {
                return (
                    arithmeticBit.none() && divisorBit.none() && integralBit.none() && optimalBit.none()
                    && primeBit.none());
            }
            void reset()
            {
                arithmeticBit.reset();
                divisorBit.reset();
                integralBit.reset();
                optimalBit.reset();
                primeBit.reset();
            }

        protected:
            friend std::ostream& operator<<(std::ostream& os, const Type& type)
            {
                switch (type)
                {
                    case Type::arithmetic:
                        os << "ARITHMETIC";
                        break;
                    case Type::divisor:
                        os << "DIVISOR";
                        break;
                    case Type::integral:
                        os << "INTEGRAL";
                        break;
                    case Type::optimal:
                        os << "OPTIMAL";
                        break;
                    case Type::prime:
                        os << "PRIME";
                        break;
                    default:
                        os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
                }
                return os;
            }
        } numericTask{};

        [[nodiscard]] bool empty() const
        {
            return (
                algorithmTask.empty() && dataStructureTask.empty() && designPatternTask.empty() && numericTask.empty());
        }
        void reset()
        {
            algorithmTask.reset();
            dataStructureTask.reset();
            designPatternTask.reset();
            numericTask.reset();
        }
        template <typename T>
        auto getBit() const
        {
            if constexpr (std::is_same_v<T, AlgorithmTask::MatchMethod>)
            {
                return algorithmTask.matchBit;
            }
            else if constexpr (std::is_same_v<T, AlgorithmTask::NotationMethod>)
            {
                return algorithmTask.notationBit;
            }
            else if constexpr (std::is_same_v<T, AlgorithmTask::SearchMethod>)
            {
                return algorithmTask.searchBit;
            }
            else if constexpr (std::is_same_v<T, AlgorithmTask::SortMethod>)
            {
                return algorithmTask.sortBit;
            }
            else if constexpr (std::is_same_v<T, DataStructureTask::LinearInstance>)
            {
                return dataStructureTask.linearBit;
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralInstance>)
            {
                return designPatternTask.behavioralBit;
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalInstance>)
            {
                return designPatternTask.creationalBit;
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralInstance>)
            {
                return designPatternTask.structuralBit;
            }
            else if constexpr (std::is_same_v<T, NumericTask::ArithmeticMethod>)
            {
                return numericTask.arithmeticBit;
            }
            else if constexpr (std::is_same_v<T, NumericTask::DivisorMethod>)
            {
                return numericTask.divisorBit;
            }
            else if constexpr (std::is_same_v<T, NumericTask::IntegralMethod>)
            {
                return numericTask.integralBit;
            }
            else if constexpr (std::is_same_v<T, NumericTask::OptimalMethod>)
            {
                return numericTask.optimalBit;
            }
            else if constexpr (std::is_same_v<T, NumericTask::PrimeMethod>)
            {
                return numericTask.primeBit;
            }
        }
        template <typename T>
        void setBit(const int index)
        {
            if constexpr (std::is_same_v<T, AlgorithmTask::MatchMethod>)
            {
                algorithmTask.matchBit.set(AlgorithmTask::MatchMethod(index));
            }
            else if constexpr (std::is_same_v<T, AlgorithmTask::NotationMethod>)
            {
                algorithmTask.notationBit.set(AlgorithmTask::NotationMethod(index));
            }
            else if constexpr (std::is_same_v<T, AlgorithmTask::SearchMethod>)
            {
                algorithmTask.searchBit.set(AlgorithmTask::SearchMethod(index));
            }
            else if constexpr (std::is_same_v<T, AlgorithmTask::SortMethod>)
            {
                algorithmTask.sortBit.set(AlgorithmTask::SortMethod(index));
            }
            else if constexpr (std::is_same_v<T, DataStructureTask::LinearInstance>)
            {
                dataStructureTask.linearBit.set(DataStructureTask::LinearInstance(index));
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralInstance>)
            {
                designPatternTask.behavioralBit.set(DesignPatternTask::BehavioralInstance(index));
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalInstance>)
            {
                designPatternTask.creationalBit.set(DesignPatternTask::CreationalInstance(index));
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralInstance>)
            {
                designPatternTask.structuralBit.set(DesignPatternTask::StructuralInstance(index));
            }
            else if constexpr (std::is_same_v<T, NumericTask::ArithmeticMethod>)
            {
                numericTask.arithmeticBit.set(NumericTask::ArithmeticMethod(index));
            }
            else if constexpr (std::is_same_v<T, NumericTask::DivisorMethod>)
            {
                numericTask.divisorBit.set(NumericTask::DivisorMethod(index));
            }
            else if constexpr (std::is_same_v<T, NumericTask::IntegralMethod>)
            {
                numericTask.integralBit.set(NumericTask::IntegralMethod(index));
            }
            else if constexpr (std::is_same_v<T, NumericTask::OptimalMethod>)
            {
                numericTask.optimalBit.set(NumericTask::OptimalMethod(index));
            }
            else if constexpr (std::is_same_v<T, NumericTask::PrimeMethod>)
            {
                numericTask.primeBit.set(NumericTask::PrimeMethod(index));
            }
        }

    protected:
        friend std::ostream& operator<<(std::ostream& os, const Category& category)
        {
            switch (category)
            {
                case Category::algorithm:
                    os << "ALGORITHM";
                    break;
                case Category::dataStructure:
                    os << "DATA STRUCTURE";
                    break;
                case Category::designPattern:
                    os << "DESIGN PATTERN";
                    break;
                case Category::numeric:
                    os << "NUMERIC";
                    break;
                default:
                    os << "UNKNOWN: " << static_cast<std::underlying_type_t<Category>>(category);
            }
            return os;
        }
    };
#pragma pack()

    using AlgorithmTask = GeneralTask::AlgorithmTask;
    using MatchMethod = AlgorithmTask::MatchMethod;
    using NotationMethod = AlgorithmTask::NotationMethod;
    using SearchMethod = AlgorithmTask::SearchMethod;
    using SortMethod = AlgorithmTask::SortMethod;
    using DataStructureTask = GeneralTask::DataStructureTask;
    using LinearInstance = DataStructureTask::LinearInstance;
    using DesignPatternTask = GeneralTask::DesignPatternTask;
    using BehavioralInstance = DesignPatternTask::BehavioralInstance;
    using CreationalInstance = DesignPatternTask::CreationalInstance;
    using StructuralInstance = DesignPatternTask::StructuralInstance;
    using NumericTask = GeneralTask::NumericTask;
    using ArithmeticMethod = NumericTask::ArithmeticMethod;
    using DivisorMethod = NumericTask::DivisorMethod;
    using IntegralMethod = NumericTask::IntegralMethod;
    using OptimalMethod = NumericTask::OptimalMethod;
    using PrimeMethod = NumericTask::PrimeMethod;
    struct TaskPlan
    {
        TaskPlan() = default;

        BasicTask basicTask;
        GeneralTask generalTask;

        [[nodiscard]] bool empty() const { return (basicTask.empty() && generalTask.empty()); }
        void reset()
        {
            basicTask.reset();
            generalTask.reset();
        };
    } taskPlan{};

    typedef void (Command::*PerformTaskFunctor)() const;
    typedef void (Command::*UpdateTaskFunctor)(const std::string&);
    using TaskCategoryName = std::string;
    using TaskTypeName = std::string;
    using TargetTaskName = std::string;
    using TargetTaskVector = std::vector<TargetTaskName>;
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    using TaskTypeTuple = std::tuple<TargetTaskVector, TaskFunctorTuple>;
    using TaskCategoryMap = std::map<TaskTypeName, TaskTypeTuple>;
    using BasicTaskMap = std::map<TaskCategoryName, PerformTaskFunctor>;
    using GeneralTaskMap = std::map<TaskCategoryName, TaskCategoryMap>;
    template <typename T>
    auto get(const TaskTypeTuple& tuple) const;
    template <typename T>
    auto get(const TaskFunctorTuple& tuple) const;
    template <typename T>
    auto getTargetTaskAttribute() const;
    inline auto getTargetTaskDetail(const int categoryIndex, const int typeIndex, const int targetIndex) const;

    // clang-format off
    const BasicTaskMap basicTaskMap{
        // - Category -+----------- Run -----------
        { "console" , &Command::printConsoleOutput },
        { "help"    , &Command::printHelpMessage   },
        { "version" , &Command::printVersionInfo   },
    };
    const GeneralTaskMap generalTaskMap{
        // --- Category ---+----- Type -----+---------------- Target ----------------+----------- Run -----------+---------- UpdateTask ----------
        // ----------------+----------------+----------------------------------------+---------------------------+--------------------------------
        { "algorithm"      , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &Command::runMatch      , &Command::updateMatchTask      }}},
                              { "notation"   , {{ "pre", "pos"                      } , { &Command::runNotation   , &Command::updateNotationTask   }}},
                              { "search"     , {{ "bin", "int", "fib"               } , { &Command::runSearch     , &Command::updateSearchTask     }}},
                              { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                                  "qui", "hea", "cou", "buc", "rad" } , { &Command::runSort       , &Command::updateSortTask       }}}}},
        { "data-structure" , {{ "linear"     , {{ "lin", "sta", "que"               } , { &Command::runLinear     , &Command::updateLinearTask     }}}}},
        { "design-pattern" , {{ "behavioral" , {{ "cha", "com", "int", "ite", "med",
                                                  "mem", "obs", "sta", "str", "tem",
                                                  "vis"                             } , { &Command::runBehavioral , &Command::updateBehavioralTask }}},
                              { "creational" , {{ "abs", "bui", "fac", "pro", "sin" } , { &Command::runCreational , &Command::updateCreationalTask }}},
                              { "structural" , {{ "ada", "bri", "com", "dec", "fac",
                                                  "fly", "pro"                      } , { &Command::runStructural , &Command::updateStructuralTask }}}}},
        { "numeric"        , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &Command::runArithmetic , &Command::updateArithmeticTask }}},
                              { "divisor"    , {{ "euc", "ste"                      } , { &Command::runDivisor    , &Command::updateDivisorTask    }}},
                              { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &Command::runIntegral   , &Command::updateIntegralTask   }}},
                              { "optimal"    , {{ "gra", "ann", "par", "gen"        } , { &Command::runOptimal    , &Command::updateOptimalTask    }}},
                              { "prime"      , {{ "era", "eul"                      } , { &Command::runPrime      , &Command::updatePrimeTask      }}}}}
        // ----------------+----------------+----------------------------------------+---------------------------+--------------------------------
    };
    // clang-format on
    static constexpr uint32_t titleWidthForPrintTask{40};
    static constexpr uint32_t maxLineNumForPrintLog{50};
    void printConsoleOutput() const;
    void printHelpMessage() const;
    void printVersionInfo() const;
    void runMatch() const;
    void updateMatchTask(const std::string& target);
    void runNotation() const;
    void updateNotationTask(const std::string& target);
    void runSearch() const;
    void updateSearchTask(const std::string& target);
    void runSort() const;
    void updateSortTask(const std::string& target);
    void runLinear() const;
    void updateLinearTask(const std::string& target);
    void runBehavioral() const;
    void updateBehavioralTask(const std::string& target);
    void runCreational() const;
    void updateCreationalTask(const std::string& target);
    void runStructural() const;
    void updateStructuralTask(const std::string& target);
    void runArithmetic() const;
    void updateArithmeticTask(const std::string& target);
    void runDivisor() const;
    void updateDivisorTask(const std::string& target);
    void runIntegral() const;
    void updateIntegralTask(const std::string& target);
    void runOptimal() const;
    void updateOptimalTask(const std::string& target);
    void runPrime() const;
    void updatePrimeTask(const std::string& target);

    typedef std::variant<num_expression::Function1, num_expression::Function2> IntegralExprTarget;
    const std::
        unordered_multimap<num_expression::ExprRange<double, double>, IntegralExprTarget, num_expression::ExprMapHash>
            integralExprMap{
                {{num_expression::Function1::range1,
                  num_expression::Function1::range2,
                  num_expression::Function1::integralExpr},
                 num_expression::Function1()},
                {{num_expression::Function2::range1,
                  num_expression::Function2::range2,
                  num_expression::Function2::integralExpr},
                 num_expression::Function2()}};
    typedef std::variant<num_expression::Griewank, num_expression::Rastrigin> OptimalExprTarget;
    const std::
        unordered_multimap<num_expression::ExprRange<double, double>, OptimalExprTarget, num_expression::ExprMapHash>
            optimalExprMap{
                {{num_expression::Griewank::range1,
                  num_expression::Griewank::range2,
                  num_expression::Griewank::optimalExpr},
                 num_expression::Griewank()},
                {{num_expression::Rastrigin::range1,
                  num_expression::Rastrigin::range2,
                  num_expression::Rastrigin::optimalExpr},
                 num_expression::Rastrigin()}};

    void enterConsoleMode() const;
    void registerOnConsole(util_console::Console& console) const;
    static void viewLogContent();
    static std::string getIconBanner();
    [[noreturn]] inline void throwExcessArgumentException();
    [[noreturn]] inline void throwUnexpectedTaskException(const std::string& info);
};

inline void Command::throwUnexpectedTaskException(const std::string& info)
{
    taskPlan.reset();
    throw std::runtime_error("Unexpected task of " + info);
}

inline void Command::throwExcessArgumentException()
{
    taskPlan.reset();
    throw std::runtime_error("Excess argument.");
}

template <typename T>
auto Command::get(const TaskTypeTuple& tuple) const
{
    if constexpr (std::is_same_v<T, TargetTaskVector>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, TaskFunctorTuple>)
    {
        return std::get<1>(tuple);
    }
}

template <typename T>
auto Command::get(const TaskFunctorTuple& tuple) const
{
    if constexpr (std::is_same_v<T, PerformTaskFunctor>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, UpdateTaskFunctor>)
    {
        return std::get<1>(tuple);
    }
}

inline auto Command::getTargetTaskDetail(const int categoryIndex, const int typeIndex, const int targetIndex) const
{
    const auto taskCategoryMap =
        std::next(std::next(generalTaskMap.cbegin(), categoryIndex)->second.cbegin(), typeIndex);
    const auto targetMethod = get<TargetTaskVector>(taskCategoryMap->second).at(targetIndex);
    const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
    return std::make_tuple(targetMethod, threadName);
}

template <typename T>
auto Command::getTargetTaskAttribute() const
{
    if constexpr (std::is_same_v<T, MatchMethod>)
    {
        return std::make_tuple(GeneralTask::Category::algorithm, AlgorithmTask::Type::match);
    }
    else if constexpr (std::is_same_v<T, NotationMethod>)
    {
        return std::make_tuple(GeneralTask::Category::algorithm, AlgorithmTask::Type::notation);
    }
    else if constexpr (std::is_same_v<T, SearchMethod>)
    {
        return std::make_tuple(GeneralTask::Category::algorithm, AlgorithmTask::Type::search);
    }
    else if constexpr (std::is_same_v<T, SortMethod>)
    {
        return std::make_tuple(GeneralTask::Category::algorithm, AlgorithmTask::Type::sort);
    }
    else if constexpr (std::is_same_v<T, LinearInstance>)
    {
        return std::make_tuple(GeneralTask::Category::dataStructure, DataStructureTask::Type::linear);
    }
    else if constexpr (std::is_same_v<T, BehavioralInstance>)
    {
        return std::make_tuple(GeneralTask::Category::designPattern, DesignPatternTask::Type::behavioral);
    }
    else if constexpr (std::is_same_v<T, CreationalInstance>)
    {
        return std::make_tuple(GeneralTask::Category::designPattern, DesignPatternTask::Type::creational);
    }
    else if constexpr (std::is_same_v<T, StructuralInstance>)
    {
        return std::make_tuple(GeneralTask::Category::designPattern, DesignPatternTask::Type::structural);
    }
    else if constexpr (std::is_same_v<T, ArithmeticMethod>)
    {
        return std::make_tuple(GeneralTask::Category::numeric, NumericTask::Type::arithmetic);
    }
    else if constexpr (std::is_same_v<T, DivisorMethod>)
    {
        return std::make_tuple(GeneralTask::Category::numeric, NumericTask::Type::divisor);
    }
    else if constexpr (std::is_same_v<T, IntegralMethod>)
    {
        return std::make_tuple(GeneralTask::Category::numeric, NumericTask::Type::integral);
    }
    else if constexpr (std::is_same_v<T, OptimalMethod>)
    {
        return std::make_tuple(GeneralTask::Category::numeric, NumericTask::Type::optimal);
    }
    else if constexpr (std::is_same_v<T, PrimeMethod>)
    {
        return std::make_tuple(GeneralTask::Category::numeric, NumericTask::Type::prime);
    }
}
