//! @file optimal.hpp
//! @author ryftchen
//! @brief The declarations (optimal) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <map>
#include <random>

//! @brief Optimal-related functions in the algorithm module.
namespace algorithm::optimal
{
//! @brief Target functions.
class Function
{
public:
    //! @brief Destroy the Function object.
    virtual ~Function() = default;

    //! @brief The operator (()) overloading of Function class.
    //! @param x - independent variable
    //! @return dependent variable
    virtual inline double operator()(const double x) const = 0;
};

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
        const double left,
        const double right,
        const double eps) = 0;
};

//! @brief Gradient descent.
class Gradient : public Optimal
{
public:
    //! @brief Construct a new Gradient object.
    //! @param func - target function
    explicit Gradient(const Function& func) : func(func){};

    //! @brief The operator (()) overloading of Gradient class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const Function& func;
    //! @brief Initial learning rate.
    static constexpr double initialLearningRate{0.01};
    //! @brief Decay.
    static constexpr double decay{0.001};
    //! @brief Loop time.
    static constexpr std::uint32_t loopTime{1000};

    //! @brief Calculate the first derivative.
    //! @param x - independent variable
    //! @param eps - precision of calculation
    //! @return value of the first derivative
    [[nodiscard]] double calculateFirstDerivative(const double x, const double eps) const;
};

//! @brief Simulated annealing.
class Annealing : public Optimal
{
public:
    //! @brief Construct a new Annealing object.
    //! @param func - target function
    explicit Annealing(const Function& func) : func(func){};

    //! @brief The operator (()) overloading of Annealing class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const Function& func;
    //! @brief Initial temperature.
    static constexpr double initialT{100.0};
    //! @brief Minimal temperature.
    static constexpr double minimalT{0.01};
    //! @brief Cooling rate.
    static constexpr double coolingRate{0.98};
    //! @brief Length of Markov chain.
    static constexpr std::uint32_t markovChain{500};
};

//! @brief Particle swarm.
class Particle : public Optimal
{
public:
    //! @brief Construct a new Particle object.
    //! @param func - target function
    explicit Particle(const Function& func) : func(func){};

    //! @brief The operator (()) overloading of Particle class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const Function& func;
    //! @brief Random engine.
    std::mt19937 engine{std::random_device{}()};
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
    static constexpr std::uint32_t size{100};
    //! @brief The number of iterations.
    static constexpr std::uint32_t numOfIteration{500};

    //! @brief Individual information in the swarm.
    struct Individual
    {
        //! @brief Construct a new Individual object.
        //! @param x - independent variable
        //! @param velocity - velocity value
        //! @param positionBest - the best position
        //! @param xFitness - fitness of independent variable
        //! @param fitnessPositionBest - fitness of the best position
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
        //! @brief Construct a new Individual object.
        Individual() = default;

        //! @brief Independent variable.
        double x{0.0};
        //! @brief Velocity value.
        double velocity{0.0};
        //! @brief The best position.
        double positionBest{0.0};
        //! @brief Fitness of independent variable.
        double xFitness{0.0};
        //! @brief Fitness of the best position.
        double fitnessPositionBest{0.0};
    };
    //! @brief Compare the value in history.
    struct Smaller
    {
        //! @brief The operator (()) overloading of Smaller class.
        //! @param left - the first value
        //! @param right - the second value
        //! @return be smaller or not
        bool operator()(const double left, const double right) const { return left < right; }
    };

    //! @brief Alias for the society information in the swarm.
    using Society = std::vector<Individual>;
    //! @brief Alias for the history information in the swarm.
    using History = std::map<double, double, Smaller>;
    //! @brief Store the information of society and history in the swarm.
    struct Storage
    {
        //! @brief Construct a new Storage object.
        //! @param society - society information
        //! @param history - history information
        Storage(
            const std::initializer_list<Society::value_type>& society,
            const std::initializer_list<History::value_type>& history) :
            society(society), history(history){};
        //! @brief Construct a new Storage object.
        Storage() = default;

        //! @brief Society information
        Society society{};
        //! @brief History information
        History history{};
    };

    //! @brief Initialize the storage in the swarm.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @return storage which includes information about society and history
    Storage storageInit(const double left, const double right);
};

//! @brief Genetic.
class Genetic : public Optimal
{
public:
    //! @brief Construct a new Genetic object.
    //! @param func - target function
    explicit Genetic(const Function& func) : func(func){};

    //! @brief The operator (()) overloading of Genetic class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<double, double>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const Function& func;
    //! @brief Random engine.
    std::mt19937 engine{std::random_device{}()};
    //! @brief The number of chromosomes.
    std::uint32_t chromosomeNum{0};
    //! @brief Crossover probability.
    static constexpr double crossPr{0.75};
    //! @brief Mutation probability.
    static constexpr double mutatePr{0.035};
    //! @brief Population size.
    static constexpr std::uint32_t size{100};
    //! @brief The number of iterations.
    static constexpr std::uint32_t numOfIteration{500};

    //! @brief Alias for the individual's chromosome in species.
    using Chromosome = std::vector<std::uint8_t>;
    //! @brief Alias for the population in species.
    using Population = std::vector<Chromosome>;
    //! @brief Decode attributes of species.
    struct DecodeAttribute
    {
        //! @brief Left endpoint.
        double lower{0.0};
        //! @brief Right endpoint.
        double upper{0.0};
        //! @brief The precision of calculation.
        double eps{0.0};
    } /** @brief A DecodeAttribute object for storing decode attributes of species. */ decodeAttr{};
    //! @brief Update species.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    void updateSpecies(const double left, const double right, const double eps);
    //! @brief The genetic code.
    //! @param chr - individual's chromosome
    void geneticCode(Chromosome& chr);
    //! @brief The genetic decode.
    //! @param chr - individual's chromosome
    //! @return decoded value
    [[nodiscard]] double geneticDecode(const Chromosome& chr) const;
    //! @brief Initialize the population.
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
    //! @brief Linear transformation of fitness.
    //! @param pop - whole population
    //! @return coefficient of the linear transformation
    std::optional<std::pair<double, double>> fitnessLinearTransformation(const Population& pop);
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
    //! @brief The probability of a possible event.
    //! @return probability from 0 to 1
    inline double probability();
    //! @brief Get a random number from 0 to the limit.
    //! @param limit - maximum random number
    //! @return random number
    inline std::uint32_t getRandomLessThanLimit(const std::uint32_t limit);
};

inline double Genetic::probability()
{
    std::uniform_real_distribution<double> pr(0.0, 1.0);
    return pr(engine);
}

inline std::uint32_t Genetic::getRandomLessThanLimit(const std::uint32_t limit)
{
    std::uniform_int_distribution<int> num(0, limit);
    return num(engine);
}
} // namespace algorithm::optimal
