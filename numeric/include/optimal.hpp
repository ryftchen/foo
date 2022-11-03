#pragma once

#include <map>
#include <random>
#include "expression.hpp"

namespace num_optimal
{
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
    explicit Gradient(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const num_expression::Expression& func;
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
    explicit Annealing(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const num_expression::Expression& func;
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
    explicit Particle(const num_expression::Expression& expr) : func(expr), seed(std::random_device{}()){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const num_expression::Expression& func;
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
    explicit Genetic(const num_expression::Expression& expr) : func(expr), seed(std::random_device{}()){};

    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const num_expression::Expression& func;
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
} // namespace num_optimal
