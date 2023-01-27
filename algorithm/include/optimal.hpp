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
//! @brief Target functions of optimization.
namespace function
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

//! @brief Function object's helper type for the visitor.
//! @tparam Ts - type of visitor
template <class... Ts>
struct FuncOverloaded : Ts...
{
    using Ts::operator()...;
};

//! @brief Explicit deduction guide for FuncOverloaded.
//! @tparam Ts - type of visitor
template <class... Ts>
FuncOverloaded(Ts...) -> FuncOverloaded<Ts...>;

//! @brief Range properties of the function.
//! @tparam T1 - type of left endpoint
//! @tparam T2 - type of right endpoint
template <typename T1, typename T2>
struct FuncRange
{
    //! @brief Construct a new FuncRange object.
    //! @param range1 - left endpoint
    //! @param range2 - light endpoint
    //! @param funcDescr - function description
    FuncRange(const T1& range1, const T2& range2, const std::string_view funcDescr) :
        range1(range1), range2(range2), funcDescr(funcDescr){};
    //! @brief Construct a new FuncRange object.
    FuncRange() = delete;
    //! @brief Left endpoint.
    T1 range1;
    //! @brief Right endpoint.
    T2 range2;
    //! @brief Function description.
    std::string_view funcDescr;

    //! @brief The operator (==) overloading of FuncRange class.
    //! @param rhs - right-hand side
    //! @return be equal or not equal
    bool operator==(const FuncRange& rhs) const
    {
        return (std::tie(rhs.range1, rhs.range2, rhs.funcDescr) == std::tie(range1, range2, funcDescr));
    }
};
//! @brief Mapping hash value for the function.
struct FuncMapHash
{
    //! @brief The operator (()) overloading of FuncMapHash class.
    //! @tparam T1 - type of left endpoint
    //! @tparam T2 - type of right endpoint
    //! @param range - range properties of the function
    //! @return hash value
    template <typename T1, typename T2>
    std::size_t operator()(const FuncRange<T1, T2>& range) const
    {
        std::size_t hash1 = std::hash<T1>()(range.range1);
        std::size_t hash2 = std::hash<T2>()(range.range2);
        std::size_t hash3 = std::hash<std::string_view>()(range.funcDescr);
        return (hash1 ^ hash2 ^ hash3);
    }
};
} // namespace function

//! @brief Alias for the independent variable.
using ValueX = double;
//! @brief Alias for the dependent variable.
using ValueY = double;

//! @brief The precision of calculation.
inline constexpr double epsilon = 1e-5;

//! @brief Solution of optimal.
class OptimalSolution
{
public:
    //! @brief Destroy the OptimalSolution object.
    virtual ~OptimalSolution() = default;

    //! @brief The operator (()) overloading of OptimalSolution class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    virtual std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) = 0;

protected:
    friend std::mt19937 getRandomSeedByTime();
};

//! @brief The gradient descent method.
class Gradient : public OptimalSolution
{
public:
    //! @brief Construct a new Gradient object.
    //! @param func - target function
    explicit Gradient(const function::Function& func);

    //! @brief The operator (()) overloading of Gradient class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const function::Function& func;
    //! @brief Initial learning rate.
    static constexpr double initialLearningRate{0.01};
    //! @brief Decay.
    static constexpr double decay{0.001};
    //! @brief Loop time.
    static constexpr uint32_t loopTime{1000};

    //! @brief Calculate the first derivative.
    //! @param x - independent variable
    //! @param eps - precision of calculation
    //! @return value of the first derivative
    [[nodiscard]] double calculateFirstDerivative(const double x, const double eps) const;
};

//! @brief The simulated annealing method.
class Annealing : public OptimalSolution
{
public:
    //! @brief Construct a new Annealing object.
    //! @param func - target function
    explicit Annealing(const function::Function& func);

    //! @brief The operator (()) overloading of Gradient class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const function::Function& func;
    //! @brief Initial temperature.
    static constexpr double initialT{100.0};
    //! @brief Minimal temperature.
    static constexpr double minimalT{0.01};
    //! @brief Cooling rate.
    static constexpr double coolingRate{0.98};
    //! @brief Length of Markov chain.
    static constexpr uint32_t markovChain{500};
};

//! @brief The particle swarm method.
class Particle : public OptimalSolution
{
public:
    //! @brief Construct a new Particle object.
    //! @param func - target function
    explicit Particle(const function::Function& func);

    //! @brief The operator (()) overloading of Gradient class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const function::Function& func;
    //! @brief Random seed.
    std::mt19937 seed;
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
    static constexpr uint32_t size{100};
    //! @brief The number of iterations.
    static constexpr uint32_t numOfIteration{500};

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
        double x;
        //! @brief Velocity value.
        double velocity;
        //! @brief The best position.
        double positionBest;
        //! @brief Fitness of independent variable.
        double xFitness;
        //! @brief Fitness of the best position.
        double fitnessPositionBest;
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
    using History = std::map<ValueY, ValueX, Smaller>;
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

        //! @brief society information
        Society society;
        //! @brief history information
        History history;
    };

    //! @brief Initialize the storage in the swarm.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @return storage which includes information about society and history
    Storage storageInit(const double left, const double right);
};

//! @brief The genetic method.
class Genetic : public OptimalSolution
{
public:
    //! @brief Construct a new Genetic object.
    //! @param func - target function
    explicit Genetic(const function::Function& func);

    //! @brief The operator (()) overloading of Gradient class.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of optimal
    [[nodiscard]] std::optional<std::tuple<ValueY, ValueX>> operator()(
        const double left,
        const double right,
        const double eps) override;

private:
    //! @brief Target function.
    const function::Function& func;
    //! @brief Range properties of species.
    struct Range
    {
        //! @brief Left endpoint.
        double lower{0.0};
        //! @brief Right endpoint.
        double upper{0.0};
        //! @brief The precision of calculation.
        double eps{0.0};
    } /** @brief A Range object for storing range properties of species. */ range{};
    //! @brief Random seed.
    std::mt19937 seed;
    //! @brief The number of chromosomes.
    uint32_t chrNum{0};
    //! @brief Crossover probability.
    static constexpr double crossPr{0.75};
    //! @brief Mutation probability.
    static constexpr double mutatePr{0.035};
    //! @brief Population size.
    static constexpr uint32_t size{100};
    //! @brief The number of iterations.
    static constexpr uint32_t numOfIteration{500};

    //! @brief Alias for the individual's chromosome in species.
    using Chromosome = std::vector<uint8_t>;
    //! @brief Alias for the population in species.
    using Population = std::vector<Chromosome>;
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
    inline uint32_t getRandomNumber(const uint32_t limit);
};

inline double Genetic::probability()
{
    std::uniform_real_distribution<double> pr(0.0, 1.0);
    return pr(seed);
}

inline uint32_t Genetic::getRandomNumber(const uint32_t limit)
{
    std::uniform_int_distribution<int> randomNumber(0, limit);
    return randomNumber(seed);
}
} // namespace algorithm::optimal
