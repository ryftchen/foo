//! @file optimal.cpp
//! @author ryftchen
//! @brief The definitions (optimal) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "optimal.hpp"

#include <algorithm>

namespace algorithm::optimal
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

Result Gradient::operator()(const double left, const double right, const double eps)
{
    if (!func)
    {
        return std::nullopt;
    }

    const auto climbing = createClimbers(left, right);
    double xBest = *climbing.cbegin();
    double yBest = func(xBest);

    for (std::uint32_t iteration = 0; auto x : climbing)
    {
        iteration = 0;
        double learningRate = initialLR;
        double gradient = calculateFirstDerivative(x, eps);
        double dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x - dx) >= left) && ((x - dx) <= right))
        {
            x -= dx;
            ++iteration;
            learningRate = initialLR / (1.0 + decay * iteration);
            gradient = calculateFirstDerivative(x, eps);
            dx = learningRate * gradient;
        }

        if (const double y = func(x); y < yBest)
        {
            xBest = x;
            yBest = y;
        }
    }
    return std::make_optional(std::make_tuple(yBest, xBest));
}

std::unordered_multiset<double> Gradient::createClimbers(const double left, const double right) const
{
    std::unordered_multiset<double> climbing{};
    climbing.reserve(loopTime);
    std::generate_n(
        std::inserter(climbing, climbing.end()),
        loopTime,
        [engine = std::mt19937_64(std::random_device{}()),
         candidate = std::uniform_real_distribution<double>(left, right)]() mutable { return candidate(engine); });
    return climbing;
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
}

Result Tabu::operator()(const double left, const double right, const double eps)
{
    if (!func)
    {
        return std::nullopt;
    }

    std::mt19937_64 engine(std::random_device{}());
    double solution = std::uniform_real_distribution<double>{left, right}(engine);
    double xBest = solution;
    double yBest = func(xBest);
    double stepLen = initialStep;

    std::vector<double> neighborhood{};
    std::vector<double> tabuList{};
    neighborhood.reserve(neighborSize);
    tabuList.reserve(tabuTenure);
    tabuList.emplace_back(solution);
    for (std::uint32_t i = 0; i < maxIterations; ++i)
    {
        updateNeighborhood(neighborhood, solution, stepLen, left, right);
        if (const auto [yBestNbr, xBestNbr] = neighborhoodSearch(neighborhood, solution, tabuList); yBestNbr < yBest)
        {
            xBest = xBestNbr;
            yBest = yBestNbr;
        }
        neighborhood.clear();

        solution = xBest;
        if (tabuList.size() == tabuTenure)
        {
            tabuList.erase(tabuList.cbegin());
        }
        tabuList.emplace_back(solution);
        stepLen *= expDecay;
        if (stepLen < eps)
        {
            break;
        }
    }
    return std::make_optional(std::make_tuple(yBest, xBest));
}

void Tabu::updateNeighborhood(
    std::vector<double>& neighborhood,
    const double solution,
    const double stepLen,
    const double left,
    const double right) const
{
    for (std::uint32_t i = 0; i < (neighborSize / 2); ++i)
    {
        const double neighbor1 = std::min(solution + (i * stepLen), right);
        neighborhood.emplace_back(neighbor1);

        const double neighbor2 = std::max(solution - (i * stepLen), left);
        neighborhood.emplace_back(neighbor2);
    }
}

std::tuple<double, double> Tabu::neighborhoodSearch(
    const std::vector<double>& neighborhood, const double solution, const std::vector<double>& tabuList)
{
    double xBestNbr = solution;
    double yBestNbr = func(xBestNbr);
    for (const auto neighbor : neighborhood)
    {
        if (std::ranges::find(tabuList, neighbor) == tabuList.cend())
        {
            if (const double fitness = func(neighbor); fitness < yBestNbr)
            {
                xBestNbr = neighbor;
                yBestNbr = fitness;
            }
        }
    }
    return std::make_tuple(yBestNbr, xBestNbr);
}

Result Annealing::operator()(const double left, const double right, const double eps)
{
    if (!func)
    {
        return std::nullopt;
    }

    std::mt19937_64 engine(std::random_device{}());
    std::uniform_real_distribution<double> perturbation(left, right);
    std::uniform_real_distribution<double> pr(0.0, 1.0);
    double temperature = initialT;
    double x = std::round(perturbation(engine) / eps) * eps;
    double y = func(x);
    double xBest = x;
    double yBest = y;

    while (temperature > minimalT)
    {
        x = xBest;
        y = yBest;
        for (std::uint32_t i = 0; i < markovChainLen; ++i)
        {
            double xNbr = cauchyLikeDistribution(x, left, right, temperature, pr(engine));
            if ((xNbr < left) || (xNbr > right))
            {
                xNbr = std::round(perturbation(engine) / eps) * eps;
            }
            if (const double yNbr = func(xNbr); metropolisAcceptanceCriterion(yNbr - y, temperature, pr(engine)))
            {
                x = xNbr;
                y = yNbr;
                if (y < yBest)
                {
                    xBest = x;
                    yBest = y;
                }
            }
        }
        temperature *= coolingRate;
    }
    return std::make_optional(std::make_tuple(yBest, xBest));
}

double Annealing::cauchyLikeDistribution(
    const double prev, const double min, const double max, const double temp, const double xi)
{
    const double sgn = static_cast<int>((xi - 0.5) > 0.0) - static_cast<int>((xi - 0.5) < 0.0);
    return prev + ((temp * sgn * (1.0 + 1.0 / (std::pow(temp, (2.0 * xi) - 1.0) - 1.0))) * (max - min));
}

bool Annealing::metropolisAcceptanceCriterion(const double deltaE, const double temp, const double xi)
{
    return (deltaE < 0.0) || (std::exp(-deltaE / temp) > xi);
}

Result Particle::operator()(const double left, const double right, const double eps)
{
    if (!func)
    {
        return std::nullopt;
    }

    auto swarm = swarmInit(left, right);
    const auto initialBest =
        std::ranges::min_element(swarm, [](const auto& min1, const auto& min2) { return min1.fitness < min2.fitness; });
    double gloBest = initialBest->x;
    double gloBestFitness = initialBest->fitness;

    for (std::uint32_t i = 0; i < maxIterations; ++i)
    {
        updateParticles(swarm, i, gloBest, left, right, eps);
        updateBests(swarm, gloBest, gloBestFitness);
    }
    return std::make_optional(std::make_tuple(gloBestFitness, gloBest));
}

Particle::Swarm Particle::swarmInit(const double left, const double right)
{
    Swarm swarm{};
    swarm.reserve(swarmSize);
    std::generate_n(
        std::back_inserter(swarm),
        swarmSize,
        [this,
         candidate = std::uniform_real_distribution<double>(left, right),
         velocity = std::uniform_real_distribution<double>(vMin, vMax)]() mutable
        {
            const double x = candidate(engine);
            return Individual{x, velocity(engine), x, func(x), func(x)};
        });
    return swarm;
}

void Particle::updateParticles(
    Swarm& swarm,
    const std::uint32_t iteration,
    const double gloBest,
    const double left,
    const double right,
    const double eps)
{
    for (const double w = nonlinearDecreasingWeight(iteration); auto& ind : swarm)
    {
        const double rand1 = std::round(perturbation(engine) / eps) * eps;
        const double rand2 = std::round(perturbation(engine) / eps) * eps;
        ind.v = w * ind.v + c1 * rand1 * (ind.persBest - ind.x) + c2 * rand2 * (gloBest - ind.x);
        ind.v = std::clamp(ind.v, vMin, vMax);

        ind.x += ind.v;
        ind.x = std::clamp(ind.x, left, right);

        ind.fitness = func(ind.x);
    }
}

double Particle::nonlinearDecreasingWeight(const std::uint32_t iteration) const
{
    return wBegin - ((wBegin - wEnd) * std::pow((1.0 + iteration) / maxIterations, 2.0));
}

void Particle::updateBests(Swarm& swarm, double& gloBest, double& gloBestFitness)
{
    for (auto& ind : swarm)
    {
        if (ind.fitness < ind.persBestFitness)
        {
            ind.persBest = ind.x;
            ind.persBestFitness = ind.fitness;
        }
        if (ind.fitness < gloBestFitness)
        {
            gloBest = ind.x;
            gloBestFitness = ind.fitness;
        }
    }
}

Result Ant::operator()(const double left, const double right, const double eps)
{
    if (!func)
    {
        return std::nullopt;
    }

    auto colony = colonyInit(left, right);
    double xBest = colony.front().position;
    double yBest = func(xBest);
    double stepLen = initialStep;

    for (std::uint32_t i = 0; i < maxIterations; ++i)
    {
        stateTransition(colony, eps);
        pathConstruction(colony, stepLen, left, right);
        updatePheromones(colony);

        const auto currBest = std::ranges::min_element(
            colony, [](const auto& min1, const auto& min2) { return min1.pheromone < min2.pheromone; });
        if (const double x = currBest->position, y = func(x); y < yBest)
        {
            xBest = x;
            yBest = y;
        }
        stepLen = initialStep / (1.0 + i);
        if (stepLen < eps)
        {
            break;
        }
    }
    return std::make_optional(std::make_tuple(yBest, xBest));
}

Ant::Colony Ant::colonyInit(const double left, const double right)
{
    Colony colony{};
    colony.reserve(numOfAnts);
    std::generate_n(
        std::back_inserter(colony),
        numOfAnts,
        [this, candidate = std::uniform_real_distribution<double>(left, right)]() mutable
        {
            const double x = candidate(engine);
            return State{x, func(x), 0.0};
        });
    return colony;
}

void Ant::stateTransition(Colony& colony, const double eps)
{
    const double minTau =
        std::ranges::min_element(
            colony, [](const auto& min1, const auto& min2) { return min1.pheromone < min2.pheromone; })
            ->pheromone;
    const double maxTau =
        std::ranges::max_element(
            colony, [](const auto& max1, const auto& max2) { return max1.pheromone < max2.pheromone; })
            ->pheromone;
    std::ranges::for_each(
        colony,
        [minTau, maxTau, eps](auto& state) { state.transPr = (maxTau - state.pheromone) / (maxTau - minTau + eps); });
}

void Ant::pathConstruction(Colony& colony, const double stepLen, const double left, const double right)
{
    for (auto& state : colony)
    {
        double pos = state.position
            + ((state.transPr < p0) ? stepLen * localCoeff(engine) : (right - left) * globalCoeff(engine));
        pos = std::clamp(pos, left, right);
        if (func(pos) < func(state.position))
        {
            state.position = pos;
        }
    }
}

void Ant::updatePheromones(Colony& colony)
{
    std::ranges::for_each(
        colony, [this](auto& state) { state.pheromone = (1.0 - rho) * state.pheromone + func(state.position); });
}

auto Genetic::getBestIndividual(const Population& pop)
{
    std::vector<double> fitness{};
    fitness.reserve(pop.size());
    std::ranges::transform(pop, std::back_inserter(fitness), [this](const auto& ind) { return calculateFitness(ind); });
    return std::next(pop.cbegin(), std::ranges::distance(fitness.cbegin(), std::ranges::max_element(fitness)));
}

Result Genetic::operator()(const double left, const double right, const double eps)
{
    if (!func || !updateSpecies(left, right, eps))
    {
        return std::nullopt;
    }

    auto pop = populationInit();
    for (std::uint32_t i = 0; i < numOfGens; ++i)
    {
        auto elite = extractElite(pop);
        select(pop);
        crossover(pop);
        mutate(pop);
        pop.emplace_back(std::move(elite));
    }
    const double xBest = geneticDecode(*getBestIndividual(pop));
    return std::make_optional(std::make_tuple(func(xBest), xBest));
}

bool Genetic::updateSpecies(const double left, const double right, const double eps)
{
    const double stateNum = ((right - left) / eps) + 1.0;
    const auto estimatedLen = static_cast<std::uint32_t>(std::ceil(std::log2(stateNum)));
    if (estimatedLen < minChrLen)
    {
        return false;
    }

    chromosomeLen = estimatedLen;
    property = Property{left, right, eps};
    return true;
}

double Genetic::geneticDecode(const Chromosome& chr) const
{
    std::uint32_t currDecoded = 0;
    std::ranges::for_each(chr, [&currDecoded](const auto bit) { currDecoded = (currDecoded << 1) | bit; });
    const auto maxDecoded = static_cast<double>((1ULL << chromosomeLen) - 1ULL);
    return property.lower + ((property.upper - property.lower) * currDecoded / maxDecoded);
}

Genetic::Population Genetic::populationInit()
{
    Population pop{};
    pop.reserve(popSize);
    std::generate_n(
        std::back_inserter(pop),
        popSize,
        [this]()
        {
            Chromosome chr{};
            chr.reserve(chromosomeLen);
            std::generate_n(
                std::back_inserter(chr),
                chromosomeLen,
                [this, bit = std::uniform_int_distribution<std::uint8_t>(0, 1)]() mutable { return bit(engine); });
            return chr;
        });
    return pop;
}

void Genetic::geneticCross(Chromosome& chr1, Chromosome& chr2)
{
    std::uniform_int_distribution<std::uint32_t> randomPos(1, chromosomeLen - 2);
    std::uint32_t point1 = randomPos(engine);
    std::uint32_t point2 = randomPos(engine);
    if (point1 > point2)
    {
        std::swap(point1, point2);
    }
    std::swap_ranges(chr1.begin() + point1, chr1.begin() + point2 + 1, chr2.begin() + point1);
}

void Genetic::crossover(Population& pop)
{
    std::vector<std::reference_wrapper<Chromosome>> selector(pop.begin(), pop.end());
    std::ranges::shuffle(selector, engine);
    for (auto chrIter = selector.cbegin(); (chrIter != selector.cend()) && (std::next(chrIter, 1) != selector.cend());
         std::advance(chrIter, 2))
    {
        if (probability(engine) < crossPr)
        {
            auto& parent1 = chrIter->get();
            auto& parent2 = std::next(chrIter, 1)->get();
            geneticCross(parent1, parent2);
        }
    }
}

void Genetic::geneticMutation(Chromosome& chr)
{
    std::ranges::for_each(
        chr,
        [this](auto& bit)
        {
            if (probability(engine) < mutatePr)
            {
                bit = !bit;
            }
        });
}

void Genetic::mutate(Population& pop)
{
    std::ranges::for_each(pop, [this](auto& ind) { geneticMutation(ind); });
}

double Genetic::calculateFitness(const Chromosome& chr)
{
    return -func(geneticDecode(chr));
}

std::optional<std::pair<double, double>> Genetic::goldbergLinearScaling(
    const std::vector<double>& fitness, const double eps)
{
    const double fitMax = *std::ranges::max_element(fitness);
    const double fitAvg = std::reduce(fitness.cbegin(), fitness.cend(), 0.0) / static_cast<double>(fitness.size());
    if (std::fabs(fitMax - fitAvg) < eps)
    {
        return std::nullopt;
    }
    const double alpha = (cMult - 1.0) * fitAvg / (fitMax - fitAvg);
    const double beta = fitAvg * (1.0 - alpha);
    return std::make_optional(std::pair<double, double>(alpha, beta));
}

auto Genetic::rouletteWheelSelection(const Population& pop, const std::vector<double>& cumFitness)
{
    const auto cumIter = std::ranges::lower_bound(cumFitness, probability(engine));
    return (cumIter != cumFitness.cend()) ? std::next(pop.cbegin(), std::distance(cumFitness.cbegin(), cumIter))
                                          : std::prev(pop.cend());
}

void Genetic::select(Population& pop)
{
    std::vector<double> fitness{};
    fitness.reserve(pop.size());
    std::ranges::transform(pop, std::back_inserter(fitness), [this](const auto& ind) { return calculateFitness(ind); });
    if (const double min = *std::ranges::min_element(fitness); min <= 0.0)
    {
        std::ranges::for_each(fitness, [min, eps = property.prec](auto& fit) { fit = fit - min + eps; });
    }

    const auto coeff = goldbergLinearScaling(fitness, property.prec)
                           .value_or(std::pair<double, double>(0.0, 1.0 / static_cast<double>(pop.size())));
    const double sum = std::accumulate(
        fitness.begin(),
        fitness.end(),
        0.0,
        [alpha = coeff.first, beta = coeff.second](const auto acc, auto& fit)
        {
            fit = alpha * fit + beta;
            return acc + fit;
        });
    std::ranges::for_each(fitness, [sum](auto& fit) { fit /= sum; });
    std::partial_sum(fitness.begin(), fitness.end(), fitness.begin());

    Population selected{};
    selected.reserve(pop.size());
    std::generate_n(
        std::back_inserter(selected),
        pop.size(),
        [this, &pop, &fitness]() { return *rouletteWheelSelection(pop, fitness); });
    pop = std::move(selected);
}

Genetic::Chromosome Genetic::extractElite(Population& pop)
{
    const auto bestIter = getBestIndividual(pop);
    auto elite = *bestIter;
    pop.erase(bestIter);
    return elite;
}
} // namespace algorithm::optimal
