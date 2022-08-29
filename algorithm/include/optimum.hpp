#pragma once

#include <map>
#include <random>
#include <tuple>
#include <vector>
#include "expression.hpp"

namespace alg_optimum
{
using ValueX = double;
using ValueY = double;

#define OPTIMUM_EPSILON 1e-5
#define OPTIMUM_RESULT(opt) "*%-9s method: Y(" #opt ")=%+.5f X=%+.5f  ==>Run time: %8.5f ms\n"

class Optimum
{
public:
    virtual ~Optimum() = default;
    virtual std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left, const double right, const double eps) = 0;
};

// Fibonacci method
#define OPTIMUM_FIBONACCI_X_1 (leftVal + fibonacci[n - 2] / fibonacci[n] * (rightVal - leftVal))
#define OPTIMUM_FIBONACCI_X_2 (leftVal + fibonacci[n - 1] / fibonacci[n] * (rightVal - leftVal))
#define OPTIMUM_FIBONACCI_MIN_COUNT 3
class Fibonacci : public Optimum
{
public:
    explicit Fibonacci(const alg_expression::Expression& express) : func(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left, const double right, const double eps) override;

private:
    const alg_expression::Expression& func;
    static void generateFibonacciNumber(std::vector<double>& fibonacci, const double max);
};

// Gradient ascent method
namespace gra_learning
{
constexpr static double initialLearningRate = 0.01;
constexpr static double decay = 0.001;
constexpr static uint32_t loopTime = 100;
} // namespace gra_learning
class Gradient : public Optimum
{
public:
    explicit Gradient(const alg_expression::Expression& express) : func(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left, const double right, const double eps) override;

private:
    const alg_expression::Expression& func;
    [[nodiscard]] double calculateFirstDerivative(const double x, const double eps) const;
};

// Simulated annealing method
#define OPTIMUM_ANNEALING_PERTURBATION 0.5
namespace ann_cooling
{
constexpr static double initialT = 100.0;
constexpr static double minimalT = 0.01;
constexpr static double coolingRate = 0.9;
constexpr static uint32_t markovChain = 100;
} // namespace ann_cooling
class Annealing : public Optimum
{
public:
    explicit Annealing(const alg_expression::Expression& express) : func(express){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left, const double right, const double eps) override;

private:
    const alg_expression::Expression& func;
};

// Particle swarm method
namespace par_swarm
{
constexpr static double c1 = 1.5;
constexpr static double c2 = 1.5;
constexpr static double wBegin = 0.9;
constexpr static double wEnd = 0.4;
constexpr static double vMax = 0.5;
constexpr static double vMin = -0.5;
constexpr static uint32_t size = 50;
constexpr static uint32_t iterNum = 100;
struct Individual
{
    Individual(
        const double x, const double velocity, const double positionBest, const double xFitness,
        const double fitnessPositionBest) :
        x(x),
        velocity(velocity), positionBest(positionBest), xFitness(xFitness),
        fitnessPositionBest(fitnessPositionBest){};
    double x;
    double velocity;
    double positionBest;
    double xFitness;
    double fitnessPositionBest;

    Individual() = default;
};
struct Greater
{
    bool operator()(const double left, const double right) const { return left > right; }
};

using Society = std::vector<par_swarm::Individual>;
using History = std::map<ValueY, ValueX, Greater>;
struct Record
{
    Record(
        const std::initializer_list<Society::value_type>& society,
        const std::initializer_list<History::value_type>& history) :
        society(society),
        history(history){};
    Society society;
    History history;

    Record() = default;
};
} // namespace par_swarm
class Particle : public Optimum
{
public:
    explicit Particle(const alg_expression::Expression& express) :
        func(express), seed(std::random_device{}()){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left, const double right, const double eps) override;

private:
    const alg_expression::Expression& func;
    std::mt19937 seed;
    par_swarm::Record recordInit(const double left, const double right);
};

// Genetic method
#define OPTIMUM_GENETIC_MIN_CHROMOSOME_NUMBER 3
namespace gen_species
{
using Chromosome = std::vector<uint32_t>;
using Population = std::vector<gen_species::Chromosome>;
constexpr static double crossPr = 0.8;
constexpr static double mutatePr = 0.05;
constexpr static uint32_t size = 50;
constexpr static uint32_t iterNum = 100;
} // namespace gen_species
class Genetic : public Optimum
{
public:
    explicit Genetic(const alg_expression::Expression& express) :
        func(express), seed(std::random_device{}()){};
    std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left, const double right, const double eps) override;

private:
    const alg_expression::Expression& func;
    struct Range
    {
        double lower{0.0};
        double upper{0.0};
        double eps{0.0};
    } range{};
    std::mt19937 seed;
    uint32_t chrNum{0};
    void updateSpecies(const double left, const double right, const double eps);
    void geneCoding(gen_species::Chromosome& chr);
    [[nodiscard]] double geneDecoding(const gen_species::Chromosome& chr) const;
    gen_species::Population populationInit();
    void geneCrossover(gen_species::Chromosome& chr1, gen_species::Chromosome& chr2);
    void crossIndividual(gen_species::Population& pop);
    void geneMutation(gen_species::Chromosome& chr);
    void mutateIndividual(gen_species::Population& pop);
    double calculateFitness(const gen_species::Chromosome& chr);
    std::optional<std::pair<double, double>> fitnessLinearTransformation(
        const gen_species::Population& pop);
    auto rouletteWheelSelection(
        const gen_species::Population& pop, const std::vector<double>& fitnessCum);
    void stochasticTournamentSelection(
        gen_species::Population& pop, const std::vector<double>& fitnessCum);
    void selectIndividual(gen_species::Population& pop);
    gen_species::Chromosome getBestIndividual(const gen_species::Population& pop);
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
} // namespace alg_optimum
