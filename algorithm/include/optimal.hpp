#pragma once

#include <map>
#include <random>

namespace algo_optimal
{
namespace function
{
class Function
{
public:
    virtual ~Function() = default;

    virtual inline double operator()(const double x) const = 0;
};

template <class... Ts>
struct FuncOverloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
FuncOverloaded(Ts...) -> FuncOverloaded<Ts...>;

template <typename T1, typename T2>
struct FuncRange
{
    FuncRange(const T1& range1, const T2& range2, const std::string_view funcStr) :
        range1(range1), range2(range2), funcStr(funcStr){};
    FuncRange() = delete;
    T1 range1;
    T2 range2;
    std::string_view funcStr;

    bool operator==(const FuncRange& range) const
    {
        return (std::tie(range.range1, range.range2, range.funcStr) == std::tie(range1, range2, funcStr));
    }
};
struct FuncMapHash
{
    template <typename T1, typename T2>
    std::size_t operator()(const FuncRange<T1, T2>& range) const
    {
        std::size_t hash1 = std::hash<T1>()(range.range1);
        std::size_t hash2 = std::hash<T2>()(range.range2);
        std::size_t hash3 = std::hash<std::string_view>()(range.funcStr);
        return (hash1 ^ hash2 ^ hash3);
    }
};
} // namespace function

using ValueX = double;
using ValueY = double;

inline constexpr double epsilon = 1e-5;

class OptimalSolution
{
public:
    virtual ~OptimalSolution() = default;

    virtual std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) = 0;
};

// Gradient Descent
namespace gradient
{
constexpr double initialLearningRate = 0.01;
constexpr double decay = 0.001;
constexpr uint32_t loopTime = 100;
} // namespace gradient

class Gradient : public OptimalSolution
{
public:
    explicit Gradient(const function::Function& func) : func(func){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const function::Function& func;
    [[nodiscard]] double calculateFirstDerivative(const double x, const double eps) const;
};

// Simulated Annealing
namespace annealing
{
constexpr double initialT = 100.0;
constexpr double minimalT = 0.01;
constexpr double coolingRate = 0.9;
constexpr uint32_t markovChain = 100;
} // namespace annealing

class Annealing : public OptimalSolution
{
public:
    explicit Annealing(const function::Function& func) : func(func){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const function::Function& func;
};

// Particle Swarm
namespace particle
{
constexpr double c1 = 1.5;
constexpr double c2 = 1.5;
constexpr double wBegin = 0.9;
constexpr double wEnd = 0.4;
constexpr double vMax = 0.5;
constexpr double vMin = -0.5;
constexpr uint32_t size = 50;
constexpr uint32_t numOfIteration = 100;

struct Individual
{
    Individual(
        const double x,
        const double velocity,
        const double positionBest,
        const double xFitness,
        const double fitnessPositionBest) :
        x(x),
        velocity(velocity),
        positionBest(positionBest),
        xFitness(xFitness),
        fitnessPositionBest(fitnessPositionBest){};
    Individual() = default;

    double x;
    double velocity;
    double positionBest;
    double xFitness;
    double fitnessPositionBest;
};

struct Smaller
{
    bool operator()(const double left, const double right) const { return left < right; }
};

using Society = std::vector<particle::Individual>;
using History = std::map<ValueY, ValueX, Smaller>;
struct Record
{
    Record(
        const std::initializer_list<Society::value_type>& society,
        const std::initializer_list<History::value_type>& history) :
        society(society), history(history){};
    Record() = default;

    Society society;
    History history;
};
} // namespace particle

class Particle : public OptimalSolution
{
public:
    explicit Particle(const function::Function& func) : func(func), seed(std::random_device{}()){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const function::Function& func;
    std::mt19937 seed;

    using Individual = particle::Individual;
    using Record = particle::Record;
    using Society = particle::Society;
    Record recordInit(const double left, const double right);
};

// Genetic
namespace genetic
{
using Chromosome = std::vector<uint8_t>;
using Population = std::vector<genetic::Chromosome>;

constexpr double crossPr = 0.8;
constexpr double mutatePr = 0.05;
constexpr uint32_t size = 50;
constexpr uint32_t numOfIteration = 100;
} // namespace genetic

class Genetic : public OptimalSolution
{
public:
    explicit Genetic(const function::Function& func) : func(func), seed(std::random_device{}()){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const function::Function& func;
    struct Range
    {
        double lower{0.0};
        double upper{0.0};
        double eps{0.0};
    } range{};
    std::mt19937 seed;
    uint32_t chrNum{0};

    using Chromosome = genetic::Chromosome;
    using Population = genetic::Population;
    void updateSpecies(const double left, const double right, const double eps);
    void geneCoding(Chromosome& chr);
    [[nodiscard]] double geneDecoding(const Chromosome& chr) const;
    Population populationInit();
    void geneCrossover(Chromosome& chr1, Chromosome& chr2);
    void crossIndividual(Population& pop);
    void geneMutation(Chromosome& chr);
    void mutateIndividual(Population& pop);
    double calculateFitness(const Chromosome& chr);
    std::optional<std::pair<double, double>> fitnessLinearTransformation(const Population& pop);
    auto rouletteWheelSelection(const Population& pop, const std::vector<double>& fitnessCum);
    void stochasticTournamentSelection(Population& pop, const std::vector<double>& fitnessCum);
    void selectIndividual(Population& pop);
    Chromosome getBestIndividual(const Population& pop);
    inline double random();
    inline uint32_t getRandomNumber(const uint32_t limit);
};

inline double Genetic::random()
{
    std::uniform_real_distribution<double> random(0.0, 1.0);
    return random(seed);
}

inline uint32_t Genetic::getRandomNumber(const uint32_t limit)
{
    std::uniform_int_distribution<int> randomX(0, limit);
    return randomX(seed);
}
} // namespace algo_optimal