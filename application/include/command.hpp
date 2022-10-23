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
    ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    mutable std::mutex commandMutex;
    std::condition_variable commandCondition;
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

        std::bitset<Bottom<Category>::value> basicTaskBit;

        [[nodiscard]] bool empty() const { return basicTaskBit.none(); }
        void reset() { basicTaskBit.reset(); }
    };

    class GeneralTask
    {
    public:
        template <class T>
        struct Bottom;

        enum Category
        {
            algorithm,
            designPattern,
            numeric
        };
        template <>
        struct Bottom<Category>
        {
            static constexpr int value = 3;
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

            enum BehavioralMethod
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
            struct Bottom<BehavioralMethod>
            {
                static constexpr int value = 11;
            };

            enum CreationalMethod
            {
                abstractFactory,
                builder,
                factoryMethod,
                prototype,
                singleton
            };
            template <>
            struct Bottom<CreationalMethod>
            {
                static constexpr int value = 5;
            };

            enum StructuralMethod
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
            struct Bottom<StructuralMethod>
            {
                static constexpr int value = 7;
            };

            std::bitset<Bottom<BehavioralMethod>::value> behavioralBit;
            std::bitset<Bottom<CreationalMethod>::value> creationalBit;
            std::bitset<Bottom<StructuralMethod>::value> structuralBit;

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
                sieve
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
                euclid,
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

            enum SieveMethod
            {
                eratosthenes,
                euler
            };
            template <>
            struct Bottom<SieveMethod>
            {
                static constexpr int value = 2;
            };

            std::bitset<Bottom<ArithmeticMethod>::value> arithmeticBit;
            std::bitset<Bottom<DivisorMethod>::value> divisorBit;
            std::bitset<Bottom<IntegralMethod>::value> integralBit;
            std::bitset<Bottom<OptimalMethod>::value> optimalBit;
            std::bitset<Bottom<SieveMethod>::value> sieveBit;

            [[nodiscard]] bool empty() const
            {
                return (
                    arithmeticBit.none() && divisorBit.none() && integralBit.none() && optimalBit.none()
                    && sieveBit.none());
            }
            void reset()
            {
                arithmeticBit.reset();
                divisorBit.reset();
                integralBit.reset();
                optimalBit.reset();
                sieveBit.reset();
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
                    case Type::sieve:
                        os << "SIEVE";
                        break;
                    default:
                        os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
                }
                return os;
            }
        } numericTask{};

        [[nodiscard]] bool empty() const
        {
            return (algorithmTask.empty() && designPatternTask.empty() && numericTask.empty());
        }
        void reset()
        {
            algorithmTask.reset();
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
            else if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralMethod>)
            {
                return designPatternTask.behavioralBit;
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalMethod>)
            {
                return designPatternTask.creationalBit;
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralMethod>)
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
            else if constexpr (std::is_same_v<T, NumericTask::SieveMethod>)
            {
                return numericTask.sieveBit;
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
            else if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralMethod>)
            {
                designPatternTask.behavioralBit.set(DesignPatternTask::BehavioralMethod(index));
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalMethod>)
            {
                designPatternTask.creationalBit.set(DesignPatternTask::CreationalMethod(index));
            }
            else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralMethod>)
            {
                designPatternTask.structuralBit.set(DesignPatternTask::StructuralMethod(index));
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
            else if constexpr (std::is_same_v<T, NumericTask::SieveMethod>)
            {
                numericTask.sieveBit.set(NumericTask::SieveMethod(index));
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

    using BasicTaskCategory = BasicTask::Category;
    template <class T>
    using BasicTaskBottom = BasicTask::Bottom<T>;
    using GeneralTaskCategory = GeneralTask::Category;
    template <class T>
    using GeneralTaskBottom = GeneralTask::Bottom<T>;
    using AlgorithmTask = GeneralTask::AlgorithmTask;
    using AlgorithmTaskType = AlgorithmTask::Type;
    template <class T>
    using AlgorithmTaskBottom = AlgorithmTask::Bottom<T>;
    using MatchMethod = AlgorithmTask::MatchMethod;
    using NotationMethod = AlgorithmTask::NotationMethod;
    using SearchMethod = AlgorithmTask::SearchMethod;
    using SortMethod = AlgorithmTask::SortMethod;
    using DesignPatternTask = GeneralTask::DesignPatternTask;
    using DesignPatternTaskType = DesignPatternTask::Type;
    template <class T>
    using DesignPatternTaskBottom = DesignPatternTask::Bottom<T>;
    using BehavioralMethod = DesignPatternTask::BehavioralMethod;
    using CreationalMethod = DesignPatternTask::CreationalMethod;
    using StructuralMethod = DesignPatternTask::StructuralMethod;
    using NumericTask = GeneralTask::NumericTask;
    using NumericTaskType = NumericTask::Type;
    template <class T>
    using NumericTaskBottom = NumericTask::Bottom<T>;
    using ArithmeticMethod = NumericTask::ArithmeticMethod;
    using DivisorMethod = NumericTask::DivisorMethod;
    using IntegralMethod = NumericTask::IntegralMethod;
    using OptimalMethod = NumericTask::OptimalMethod;
    using SieveMethod = NumericTask::SieveMethod;
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
    using TaskMethodName = std::string;
    using TaskMethodVector = std::vector<TaskMethodName>;
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    using TaskTypeTuple = std::tuple<TaskMethodVector, TaskFunctorTuple>;
    using TaskCategoryMap = std::map<TaskTypeName, TaskTypeTuple>;
    using BasicTaskMap = std::map<TaskCategoryName, PerformTaskFunctor>;
    using GeneralTaskMap = std::map<TaskCategoryName, TaskCategoryMap>;
    template <typename T>
    auto get(const TaskTypeTuple& tuple) const;
    template <typename T>
    auto get(const TaskFunctorTuple& tuple) const;
    template <typename T>
    auto getMethodAttribute() const;
    auto inline getMethodDetail(const int categoryIndex, const int typeIndex, const int methodIndex) const;

    // clang-format off
    static constexpr std::string_view optionTreeOfHelpMsg{
        "├── -a --algorithm\r\n"
        "│   ├── match\r\n"
        "│   │   └── rab, knu, boy, hor, sun\r\n"
        "│   ├── notation\r\n"
        "│   │   └── pre, pos\r\n"
        "│   ├── search\r\n"
        "│   │   └── bin, int, fib\r\n"
        "│   └── sort\r\n"
        "│       ├── bub, sel, ins, she, mer\r\n"
        "│       └── qui, hea, cou, buc, rad\r\n"
        "├── -dp --design-pattern\r\n"
        "│   ├── behavioral\r\n"
        "│   │   ├── cha, com, int, ite, med\r\n"
        "│   │   ├── mem, obs, sta, str, tem\r\n"
        "│   │   └── vis\r\n"
        "│   ├── creational\r\n"
        "│   │   └── abs, bui, fac, pro, sin\r\n"
        "│   └── structural\r\n"
        "│       ├── ada, bri, com, dec, fac\r\n"
        "│       └── fly, pro\r\n"
        "└── -n --numeric\r\n"
        "    ├── arithmetic\r\n"
        "    │   └── add, sub, mul, div\r\n"
        "    ├── divisor\r\n"
        "    │   └── euc, ste\r\n"
        "    ├── integral\r\n"
        "    │   └── tra, sim, rom, gau, mon\r\n"
        "    ├── optimal\r\n"
        "    │   └── gra, ann, par, gen\r\n"
        "    └── sieve\r\n"
        "        └── era, eul"
    };
    const BasicTaskMap basicTaskMap{
        // - Category -+----------- Run -----------
        { "console" , &Command::printConsoleOutput },
        { "help"    , &Command::printHelpMessage   },
        { "version" , &Command::printVersionInfo   },
    };
    const GeneralTaskMap generalTaskMap{
        // --- Category ---+----- Type -----+---------------- Method ----------------+----------- Run -----------+---------- UpdateTask ----------
        // ----------------+----------------+----------------------------------------+---------------------------+--------------------------------
        { "algorithm"      , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &Command::runMatch      , &Command::updateMatchTask      }}},
                              { "notation"   , {{ "pre", "pos"                      } , { &Command::runNotation   , &Command::updateNotationTask   }}},
                              { "search"     , {{ "bin", "int", "fib"               } , { &Command::runSearch     , &Command::updateSearchTask     }}},
                              { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                                  "qui", "hea", "cou", "buc", "rad" } , { &Command::runSort       , &Command::updateSortTask       }}}}},
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
                              { "sieve"      , {{ "era", "eul"                      } , { &Command::runSieve      , &Command::updateSieveTask      }}}}}
        // ----------------+----------------+----------------------------------------+---------------------------+--------------------------------
    };
    // clang-format on
    static constexpr uint32_t titleWidthForPrintTask{40};
    static constexpr uint32_t maxLineNumForPrintLog{50};
    void printConsoleOutput() const;
    void printHelpMessage() const;
    void printVersionInfo() const;
    void runMatch() const;
    void updateMatchTask(const std::string& method);
    void runNotation() const;
    void updateNotationTask(const std::string& method);
    void runSearch() const;
    void updateSearchTask(const std::string& method);
    void runSort() const;
    void updateSortTask(const std::string& method);
    void runBehavioral() const;
    void updateBehavioralTask(const std::string& method);
    void runCreational() const;
    void updateCreationalTask(const std::string& method);
    void runStructural() const;
    void updateStructuralTask(const std::string& method);
    void runArithmetic() const;
    void updateArithmeticTask(const std::string& method);
    void runDivisor() const;
    void updateDivisorTask(const std::string& method);
    void runIntegral() const;
    void updateIntegralTask(const std::string& method);
    void runOptimal() const;
    void updateOptimalTask(const std::string& method);
    void runSieve() const;
    void updateSieveTask(const std::string& method);

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
    [[noreturn]] void inline throwExcessArgumentException();
    [[noreturn]] void inline throwUnexpectedMethodException(const std::string& info);
};

void inline Command::throwUnexpectedMethodException(const std::string& info)
{
    taskPlan.reset();
    throw std::runtime_error("Unexpected method of " + info);
}

void inline Command::throwExcessArgumentException()
{
    taskPlan.reset();
    throw std::runtime_error("Excess argument.");
}

template <typename T>
auto Command::get(const TaskTypeTuple& tuple) const
{
    if constexpr (std::is_same_v<T, TaskMethodVector>)
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

auto inline Command::getMethodDetail(const int categoryIndex, const int typeIndex, const int methodIndex) const
{
    const auto taskCategoryMap =
        std::next(std::next(generalTaskMap.cbegin(), categoryIndex)->second.cbegin(), typeIndex);
    const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(methodIndex);
    const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
    return std::make_tuple(targetMethod, threadName);
}

template <typename T>
auto Command::getMethodAttribute() const
{
    if constexpr (std::is_same_v<T, MatchMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::algorithm, AlgorithmTaskType::match);
    }
    else if constexpr (std::is_same_v<T, NotationMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::algorithm, AlgorithmTaskType::notation);
    }
    else if constexpr (std::is_same_v<T, SearchMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::algorithm, AlgorithmTaskType::search);
    }
    else if constexpr (std::is_same_v<T, SortMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::algorithm, AlgorithmTaskType::sort);
    }
    else if constexpr (std::is_same_v<T, BehavioralMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::designPattern, DesignPatternTaskType::behavioral);
    }
    else if constexpr (std::is_same_v<T, CreationalMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::designPattern, DesignPatternTaskType::creational);
    }
    else if constexpr (std::is_same_v<T, StructuralMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::designPattern, DesignPatternTaskType::structural);
    }
    else if constexpr (std::is_same_v<T, ArithmeticMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::numeric, NumericTaskType::arithmetic);
    }
    else if constexpr (std::is_same_v<T, DivisorMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::numeric, NumericTaskType::divisor);
    }
    else if constexpr (std::is_same_v<T, IntegralMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::numeric, NumericTaskType::integral);
    }
    else if constexpr (std::is_same_v<T, OptimalMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::numeric, NumericTaskType::optimal);
    }
    else if constexpr (std::is_same_v<T, SieveMethod>)
    {
        return std::make_tuple(GeneralTaskCategory::numeric, NumericTaskType::sieve);
    }
}
