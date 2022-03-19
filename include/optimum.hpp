#pragma once
#include <map>
#include <random>
#include <tuple>
#include <vector>
#include "expression.hpp"

using ValueX = double;
using ValueY = double;

#define OPTIMUM_EPSILON 1e-5
#define OPTIMUM_RUN_BEGIN "\r\n---------- BEGIN OPTIMUM  ----------"
#define OPTIMUM_RUN_END "\r\n----------  END OPTIMUM   ----------"
#define OPTIMUM_FIBONACCI "*Fibonacci method: Y(max)=%+.5f X=%+.5f  ==>Run time: %8.5fms\n"
#define OPTIMUM_GRADIENT "*Gradient  method: Y(max)=%+.5f X=%+.5f  ==>Run time: %8.5fms\n"
#define OPTIMUM_ANNEALING "*Annealing method: Y(max)=%+.5f X=%+.5f  ==>Run time: %8.5fms\n"
#define OPTIMUM_PARTICLE "*Particle  method: Y(max)=%+.5f X=%+.5f  ==>Run time: %8.5fms\n"
#define OPTIMUM_GENETIC "*Genetic   method: Y(max)=%+.5f X=%+.5f  ==>Run time: %8.5fms\n"

class Optimum
{
public:
    virtual ~Optimum(){};
    virtual std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) = 0;
};

// Fibonacci method
#define OPTIMUM_FIBONACCI_X_1 (leftVal + fibonacci[n - 2] / fibonacci[n] * (rightVal - leftVal))
#define OPTIMUM_FIBONACCI_X_2 (leftVal + fibonacci[n - 1] / fibonacci[n] * (rightVal - leftVal))
#define OPTIMUM_FIBONACCI_MAX_UNCHANGED 3
class Fibonacci : public Optimum
{
public:
    explicit Fibonacci(const Expression &express) : fun(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const Expression &fun;
    std::optional<std::pair<double, double>> fibonacciSearch(
        const double left,
        const double right,
        const double eps);
    static void generateFibonacciNumber(std::vector<double> &fibonacci, const double max);
};

// Gradient ascent method
namespace Learning
{
static const double initLearningRate = 0.01;
static const double decay = 0.001;
static const uint32_t loopTime = 100;
} // namespace Learning
class Gradient : public Optimum
{
public:
    explicit Gradient(const Expression &express) : fun(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const Expression &fun;
    [[nodiscard]] double calculateFirstDerivative(const double x, const double eps) const;
};

// Simulated annealing method
#define OPTIMUM_ANNEALING_PERTURBATION 0.5
namespace Cooling
{
static const double initT = 100.0;
static const double minT = 0.01;
static const double coolingRate = 0.9;
static const uint32_t markovChain = 100;
} // namespace Cooling
class Annealing : public Optimum
{
public:
    explicit Annealing(const Expression &express) : fun(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const Expression &fun;
};

// Particle swarm method
namespace Swarm
{
struct Individual
{
    Individual(
        const double x,
        const double velocity,
        const double positionBest,
        const double xFitness,
        const double fitnessPositionBest)
        : x(x)
        , velocity(velocity)
        , positionBest(positionBest)
        , xFitness(xFitness)
        , fitnessPositionBest(fitnessPositionBest){};
    double x;
    double velocity;
    double positionBest;
    double xFitness;
    double fitnessPositionBest;

    Individual() = delete;
};
struct Greater
{
    bool operator()(const double left, const double right) const { return left > right; }
};
using Society = std::vector<Swarm::Individual>;
using History = std::map<double, double, Greater>;
struct Record
{
    Record(
        const std::initializer_list<Society::value_type> &society,
        const std::initializer_list<History::value_type> &history)
        : society(society), history(history){};
    Society society;
    History history;

    Record() = delete;
};
static const double c1 = 1.5;
static const double c2 = 1.5;
static const double wBegin = 0.9;
static const double wEnd = 0.4;
static const double vMax = 0.5;
static const double vMin = -0.5;
static const uint32_t size = 50;
static const uint32_t iterNum = 100;
} // namespace Swarm
class Particle : public Optimum
{
public:
    explicit Particle(const Expression &express) : fun(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const Expression &fun;
    std::mt19937 seed;
    Swarm::Record recordInit(const double left, const double right);
};

// Genetic method
namespace Species
{
using Chromosome = std::vector<uint32_t>;
using Population = std::vector<Species::Chromosome>;
static const double crossPr = 0.7;
static const double mutatePr = 0.05;
static const uint32_t size = 50;
static const uint32_t iterNum = 100;
} // namespace Species
class Genetic : public Optimum
{
public:
    explicit Genetic(const Expression &express) : fun(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    const Expression &fun;
    struct Range
    {
        double left;
        double right;
        double eps;

        Range() = default;
    } range;
    std::mt19937 seed;
    uint32_t chrNum = 0;
    void setSpecies(const double left, const double right, const double eps);
    void geneCoding(Species::Chromosome &chr);
    double geneDecoding(const Species::Chromosome &chr);
    Species::Population populationInit();
    void geneCrossover(Species::Chromosome &chr1, Species::Chromosome &chr2);
    void crossIndividual(Species::Population &pop);
    void geneMutation(Species::Chromosome &chr);
    void mutateIndividual(Species::Population &pop);
    double calculateFitness(const Species::Chromosome &chr);
    std::optional<std::tuple<double, double>> fitnessLinearTransformation(
        const Species::Population &pop);
    void stochasticTournamentSelection(
        Species::Population &pop,
        const std::vector<double> &fitnessCum);
    void selectIndividual(Species::Population &pop);
    Species::Chromosome getBestIndividual(const Species::Population &pop);
    double inline random();
    uint32_t inline getRandomNumber(const uint32_t limit);
};
double inline Genetic::random()
{
    std::uniform_real_distribution<double> random(0.0, 1.0);
    return random(seed);
}
uint32_t inline Genetic::getRandomNumber(const uint32_t limit)
{
    std::uniform_int_distribution<int> randomX(0, limit);
    return randomX(seed);
}
