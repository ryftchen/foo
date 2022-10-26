#pragma once

#include <map>
#include <random>
#include "expression.hpp"

namespace num_optimal
{
using ValueX = double;
using ValueY = double;

inline constexpr double epsilon = 1e-5;

class Optimal
{
public:
    virtual ~Optimal() = default;

    virtual std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) = 0;
};

// Gradient ascent method
namespace gradient_learning
{
constexpr double initialLearningRate = 0.01;
constexpr double decay = 0.001;
constexpr uint32_t loopTime = 100;
} // namespace gradient_learning
class Gradient : public Optimal
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

// Simulated annealing method
namespace annealing_cooling
{
constexpr double initialT = 100.0;
constexpr double minimalT = 0.01;
constexpr double coolingRate = 0.9;
constexpr uint32_t markovChain = 100;
} // namespace annealing_cooling
class Annealing : public Optimal
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

// Particle swarm method
namespace particle_swarm
{
constexpr double c1 = 1.5;
constexpr double c2 = 1.5;
constexpr double wBegin = 0.9;
constexpr double wEnd = 0.4;
constexpr double vMax = 0.5;
constexpr double vMin = -0.5;
constexpr uint32_t size = 50;
constexpr uint32_t iterNum = 100;
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

using Society = std::vector<particle_swarm::Individual>;
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
} // namespace particle_swarm
class Particle : public Optimal
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
    particle_swarm::Record recordInit(const double left, const double right);
};

// Genetic method
namespace genetic_species
{
using Chromosome = std::vector<uint8_t>;
using Population = std::vector<genetic_species::Chromosome>;

constexpr double crossPr = 0.8;
constexpr double mutatePr = 0.05;
constexpr uint32_t size = 50;
constexpr uint32_t iterNum = 100;
} // namespace genetic_species
class Genetic : public Optimal
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

    void updateSpecies(const double left, const double right, const double eps);
    void geneCoding(genetic_species::Chromosome& chr);
    [[nodiscard]] double geneDecoding(const genetic_species::Chromosome& chr) const;
    genetic_species::Population populationInit();
    void geneCrossover(genetic_species::Chromosome& chr1, genetic_species::Chromosome& chr2);
    void crossIndividual(genetic_species::Population& pop);
    void geneMutation(genetic_species::Chromosome& chr);
    void mutateIndividual(genetic_species::Population& pop);
    double calculateFitness(const genetic_species::Chromosome& chr);
    std::optional<std::pair<double, double>> fitnessLinearTransformation(const genetic_species::Population& pop);
    auto rouletteWheelSelection(const genetic_species::Population& pop, const std::vector<double>& fitnessCum);
    void stochasticTournamentSelection(genetic_species::Population& pop, const std::vector<double>& fitnessCum);
    void selectIndividual(genetic_species::Population& pop);
    genetic_species::Chromosome getBestIndividual(const genetic_species::Population& pop);
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
