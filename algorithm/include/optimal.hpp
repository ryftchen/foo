//! @file optimal.hpp
//! @author ryftchen
//! @brief The declarations (optimal) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <optional>
#include <random>
#include <unordered_set>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Optimal-related functions in the algorithm module.
namespace optimal
{
extern const char* version() noexcept;

//! @brief Target functions.
using Function = std::function<double(const double)>;
//! @brief The precision of calculation.
inline constexpr double epsilon = 1e-5;

//! @brief Optimal methods.
class Optimal
{
public:
    //! @brief Destroy the Optimal object.
    virtual ~Optimal() = default;

    //! @brief The operator (()) overloading of Optimal class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    virtual std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) = 0;

protected:
    //! @brief Construct a new Optimal object.
    Optimal() = default;
};

//! @brief Gradient descent (GD).
class Gradient : public Optimal
{
public:
    //! @brief Construct a new Gradient object.
    //! @param func - target function
    explicit Gradient(Function func) : func{std::move(func)} {}

    //! @brief The operator (()) overloading of Gradient class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) override;

private:
    //! @brief Target function.
    const Function func{};
    //! @brief Initial learning rate.
    static constexpr double initialLearningRate{0.01};
    //! @brief Decay.
    static constexpr double decay{0.001};
    //! @brief Loop time.
    static constexpr std::uint32_t loopTime{1000};

    //! @brief Create climbers.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @return collection of climbers
    static std::unordered_multiset<double> createClimbers(const double left, const double right);
    //! @brief Calculate the first derivative.
    //! @param x - independent variable
    //! @param eps - precision of calculation
    //! @return value of the first derivative
    [[nodiscard]] double calculateFirstDerivative(const double x, const double eps) const;
};

//! @brief Tabu search (TS).
class Tabu : public Optimal
{
public:
    //! @brief Construct a new Tabu object.
    //! @param func - target function
    explicit Tabu(Function func) : func{std::move(func)} {}

    //! @brief The operator (()) overloading of Tabu class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) override;

private:
    //! @brief Target function.
    const Function func{};
    //! @brief Tabu tenure.
    static constexpr std::uint32_t tabuTenure{50};
    //! @brief Initial step length.
    static constexpr double initialStep{1.0};
    //! @brief Exponential decay of step length.
    static constexpr double expDecay{0.95};
    //! @brief Size of the neighborhood.
    static constexpr std::uint32_t neighborSize{100};
    //! @brief Maximum number of iterations.
    static constexpr std::uint32_t maxIterations{1000};

    //! @brief Update the neighborhood.
    //! @param neighborhood - neighborhood of solution
    //! @param solution - current solution
    //! @param stepLen - step length
    //! @param left - left endpoint
    //! @param right - right endpoint
    static void updateNeighborhood(
        std::vector<double>& neighborhood,
        const double solution,
        const double stepLen,
        const double left,
        const double right);
    //! @brief Neighborhood search.
    //! @param neighborhood - neighborhood of solution
    //! @param solution - current solution
    //! @param tabuList - tabu list
    //! @return best fitness and solution
    std::tuple<double, double> neighborhoodSearch(
        const std::vector<double>& neighborhood, const double solution, const std::vector<double>& tabuList);
};

//! @brief Simulated annealing (SA).
class Annealing : public Optimal
{
public:
    //! @brief Construct a new Annealing object.
    //! @param func - target function
    explicit Annealing(Function func) : func{std::move(func)} {}

    //! @brief The operator (()) overloading of Annealing class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) override;

private:
    //! @brief Target function.
    const Function func{};
    //! @brief Initial temperature.
    static constexpr double initialT{100.0};
    //! @brief Minimal temperature.
    static constexpr double minimalT{0.01};
    //! @brief Cooling rate.
    static constexpr double coolingRate{0.99};
    //! @brief Length of Markov chain.
    static constexpr std::uint32_t markovChainLength{200};

    //! @brief Temperature-dependent Cauchy-like distribution.
    //! @param prev - current model
    //! @param min - minimum of model
    //! @param max - maximum of model
    //! @param temp - current temperature
    //! @param xi - random number in the interval [0, 1]
    //! @return new model
    static double cauchyLikeDistribution(
        const double prev, const double min, const double max, const double temp, const double xi);
    //! @brief Metropolis acceptance criterion which based on the Metropolis-Hastings algorithm.
    //! @param deltaE - energy difference
    //! @param temp - current temperature
    //! @param xi - random number in the interval [0, 1]
    //! @return accept or not
    static bool metropolisAcceptanceCriterion(const double deltaE, const double temp, const double xi);
};

//! @brief Particle swarm optimization (PSO).
class Particle : public Optimal
{
public:
    //! @brief Construct a new Particle object.
    //! @param func - target function
    explicit Particle(Function func) : func{std::move(func)} {}

    //! @brief The operator (()) overloading of Particle class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) override;

private:
    //! @brief Target function.
    const Function func{};
    //! @brief Random engine.
    std::mt19937_64 engine{std::random_device{}()};
    //! @brief The perturbation for the coefficient (from 0 to 1).
    std::uniform_real_distribution<double> perturbation{0.0, 1.0};
    //! @brief Cognitive coefficient.
    static constexpr double c1{1.5};
    //! @brief Social coefficient.
    static constexpr double c2{1.5};
    //! @brief Inertia weight beginning value.
    static constexpr double wBegin{0.85};
    //! @brief Inertia weight ending value.
    static constexpr double wEnd{0.35};
    //! @brief Maximum velocity.
    static constexpr double vMax{0.5};
    //! @brief Minimum velocity.
    static constexpr double vMin{-0.5};
    //! @brief Swarm size.
    static constexpr std::uint32_t swarmSize{100};
    //! @brief Maximum number of iterations.
    static constexpr std::uint32_t maxIterations{100};

    //! @brief Individual information in the swarm.
    struct Individual
    {
        //! @brief Position vector.
        double x{0.0};
        //! @brief Velocity vector.
        double v{0.0};
        //! @brief Personal best position.
        double persBest{0.0};
        //! @brief Fitness of the position vector.
        double fitness{0.0};
        //! @brief Fitness of the personal best position.
        double persBestFitness{0.0};
    };
    //! @brief Alias for the swarm information.
    using Swarm = std::vector<Individual>;
    //! @brief Initialize the swarm.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @return initial swarm
    Swarm swarmInit(const double left, const double right);
    //! @brief Update the velocity and position of each particle in the swarm.
    //! @param swarm - particle swarm
    //! @param iteration - current number of iterations
    //! @param gloBest - global best position
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    void updateParticles(
        Swarm& swarm,
        const std::uint32_t iteration,
        const double gloBest,
        const double left,
        const double right,
        const double eps);
    //! @brief Non-linear decreasing weight.
    //! @param iteration - current number of iterations
    //! @return inertia weight
    static double nonlinearDecreasingWeight(const std::uint32_t iteration);
    //! @brief Update the personal best position of each particle in the swarm and the global best position.
    //! @param swarm - particle swarm
    //! @param gloBest - global best position
    //! @param gloBestFitness - fitness of the global best position
    static void updateBests(Swarm& swarm, double& gloBest, double& gloBestFitness);
};

//! @brief Ant colony optimization (ACO).
class Ant : public Optimal
{
public:
    //! @brief Construct a new Ant object.
    //! @param func - target function
    explicit Ant(Function func) : func{std::move(func)} {}

    //! @brief The operator (()) overloading of Ant class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) override;

private:
    //! @brief Target function.
    const Function func{};
    //! @brief Random engine.
    std::mt19937_64 engine{std::random_device{}()};
    //! @brief Coefficient of the step length for the local search.
    std::uniform_real_distribution<double> localCoeff{-1.0, 1.0};
    //! @brief Coefficient of the range for the global search.
    std::uniform_real_distribution<double> globalCoeff{-0.5, 0.5};
    //! @brief Pheromone evaporation rate.
    static constexpr double rho{0.9};
    //! @brief Exploration probability.
    static constexpr double p0{0.2};
    //! @brief Initial step length.
    static constexpr double initialStep{1.0};
    //! @brief Number of ants.
    static constexpr std::uint32_t numOfAnts{500};
    //! @brief Maximum number of iterations.
    static constexpr std::uint32_t maxIterations{500};

    //! @brief State of the ant in the colony.
    struct State
    {
        //! @brief Position coordinate.
        double position{0.0};
        //! @brief Pheromone intensity level.
        double pheromone{0.0};
        //! @brief Transition probability.
        double transPr{0.0};
    };
    //! @brief Alias for the colony information.
    using Colony = std::vector<State>;
    //! @brief Initialize the colony.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @return initial colony
    Colony colonyInit(const double left, const double right);
    //! @brief Perform the state transition for each ant in the colony.
    //! @param colony - ant colony
    //! @param eps - precision of calculation
    static void stateTransition(Colony& colony, const double eps);
    //! @brief Construct the paths of the ants in the search space.
    //! @param colony - ant colony
    //! @param stepLen - step length
    //! @param left - left endpoint
    //! @param right - right endpoint
    void pathConstruction(Colony& colony, const double stepLen, const double left, const double right);
    //! @brief Update the pheromone intensity levels.
    //! @param colony - ant colony
    void updatePheromones(Colony& colony);
};

//! @brief Genetic algorithm (GA).
class Genetic : public Optimal
{
public:
    //! @brief Construct a new Genetic object.
    //! @param func - target function
    explicit Genetic(Function func) : func{std::move(func)} {}

    //! @brief The operator (()) overloading of Genetic class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left, const double right, const double eps) override;

private:
    //! @brief Target function.
    const Function func{};
    //! @brief Random engine.
    std::mt19937_64 engine{std::random_device{}()};
    //! @brief The probability of a possible event (from 0 to 1).
    std::uniform_real_distribution<double> probability{0.0, 1.0};
    //! @brief Crossover probability.
    static constexpr double crossPr{0.7};
    //! @brief Mutation probability.
    static constexpr double mutatePr{0.001};
    //! @brief Population size.
    static constexpr std::uint32_t popSize{500};
    //! @brief Number of generations.
    static constexpr std::uint32_t numOfGenerations{20};
    //! @brief The linear scaling coefficient.
    static constexpr double cMult{1.01};
    //! @brief Minimum length of chromosome.
    static constexpr std::uint32_t minChrLen{2};
    //! @brief Length of chromosome.
    std::uint32_t chromosomeLength{0};
    //! @brief Properties of species.
    struct Property
    {
        //! @brief Left endpoint.
        double lower{0.0};
        //! @brief Right endpoint.
        double upper{0.0};
        //! @brief The precision of calculation.
        double prec{0.0};
    } /** @brief A Property object for storing properties of species. */ property{};

    //! @brief Update species.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    void updateSpecies(const double left, const double right, const double eps);
    //! @brief Alias for the individual's chromosome in species.
    using Chromosome = std::vector<std::uint8_t>;
    //! @brief Alias for the population in species.
    using Population = std::vector<Chromosome>;
    //! @brief The genetic decode.
    //! @param chr - individual's chromosome
    //! @return decoded value
    [[nodiscard]] double geneticDecode(const Chromosome& chr) const;
    //! @brief Initialize the population with binary.
    //! @return initial population
    Population populationInit();
    //! @brief The genetic cross.
    //! @param chr1 - the chromosome from one of the parents
    //! @param chr2 - the chromosome from one of the parents
    void geneticCross(Chromosome& chr1, Chromosome& chr2);
    //! @brief Chromosomal crossover in the population.
    //! @param pop - whole population
    void crossover(Population& pop);
    //! @brief The genetic mutation.
    //! @param chr - individual's chromosome
    void geneticMutation(Chromosome& chr);
    //! @brief Chromosomal mutation in the population.
    //! @param pop - whole population
    void mutate(Population& pop);
    //! @brief Calculate the fitness of the individual.
    //! @param chr - individual's chromosome
    //! @return fitness of the individual
    double calculateFitness(const Chromosome& chr);
    //! @brief The Goldberg linear scaling.
    //! @param fitness - original fitness
    //! @param eps - precision of calculation
    //! @return coefficient of the linear transformation
    static std::optional<std::pair<double, double>> goldbergLinearScaling(
        const std::vector<double>& fitness, const double eps);
    //! @brief The roulette wheel selection.
    //! @param pop - whole population
    //! @param cumFitness - cumulative fitness
    //! @return selected competitor
    auto rouletteWheelSelection(const Population& pop, const std::vector<double>& cumFitness);
    //! @brief The stochastic tournament selection.
    //! @param pop - whole population
    //! @param cumFitness - cumulative fitness
    void stochasticTournamentSelection(Population& pop, const std::vector<double>& cumFitness);
    //! @brief The genetic selection.
    //! @param pop - whole population
    void select(Population& pop);
    //! @brief Get the best individual.
    //! @param pop - whole population
    //! @return the best individual's chromosome
    Chromosome getBestIndividual(const Population& pop);
};
} // namespace optimal
} // namespace algorithm
