//! @file optimal.cpp
//! @author ryftchen
//! @brief The definitions (optimal) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "optimal.hpp"

#include <algorithm>
#include <stdexcept>

namespace algorithm::optimal
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::optional<std::tuple<double, double>> Gradient::operator()(const double left, const double right, const double eps)
{
    const auto climbing = createClimbers(left, right);
    double xBest = *climbing.cbegin(), yBest = func(xBest);

    for (std::uint32_t iteration = 0; auto x : climbing)
    {
        iteration = 0;
        double learningRate = initialLR, gradient = calculateFirstDerivative(x, eps), dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x - dx) >= left) && ((x - dx) <= right))
        {
            x -= dx;
            ++iteration;
            learningRate = initialLR * 1.0 / (1.0 + decay * iteration);
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
    std::mt19937_64 engine(std::random_device{}());
    std::uniform_real_distribution<double> candidate(left, right);
    std::unordered_multiset<double> climbing{};
    climbing.reserve(loopTime);
    while (climbing.size() < loopTime)
    {
        climbing.emplace(candidate(engine));
    }

    return climbing;
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
}

std::optional<std::tuple<double, double>> Tabu::operator()(const double left, const double right, const double eps)
{
    std::mt19937_64 engine(std::random_device{}());
    double solution = std::uniform_real_distribution<double>{left, right}(engine), xBest = solution,
           yBest = func(xBest), stepLen = initialStep;

    std::vector<double> neighborhood{}, tabuList{};
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
    for (std::uint32_t i = 0; i < neighborSize / 2; ++i)
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
    double xBestNbr = solution, yBestNbr = func(xBestNbr);
    for (const auto neighbor : neighborhood)
    {
        if (std::find(tabuList.cbegin(), tabuList.cend(), neighbor) == tabuList.cend())
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

std::optional<std::tuple<double, double>> Annealing::operator()(const double left, const double right, const double eps)
{
    std::mt19937_64 engine(std::random_device{}());
    std::uniform_real_distribution<double> perturbation(left, right), pr(0.0, 1.0);
    double temperature = initialT, x = std::round(perturbation(engine) / eps) * eps, y = func(x), xBest = x, yBest = y;

    while (temperature > minimalT)
    {
        x = xBest;
        y = yBest;
        for (std::uint32_t i = 0; i < markovChainLength; ++i)
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
    const double sgn = ((xi - 0.5) > 0) - ((xi - 0.5) < 0);
    return prev + ((temp * sgn * (1 + 1 / (std::pow(temp, (2 * xi) - 1) - 1))) * (max - min));
}

bool Annealing::metropolisAcceptanceCriterion(const double deltaE, const double temp, const double xi)
{
    return (deltaE < 0) || (std::exp(-deltaE / temp) > xi);
}

std::optional<std::tuple<double, double>> Particle::operator()(const double left, const double right, const double eps)
{
    auto swarm = swarmInit(left, right);
    const auto initialBest = std::min_element(
        swarm.cbegin(), swarm.cend(), [](const auto& min1, const auto& min2) { return min1.fitness < min2.fitness; });
    double gloBest = initialBest->x, gloBestFitness = initialBest->fitness;

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
        const double rand1 = std::round(perturbation(engine) / eps) * eps,
                     rand2 = std::round(perturbation(engine) / eps) * eps;
        ind.v = w * ind.v + c1 * rand1 * (ind.persBest - ind.x) + c2 * rand2 * (gloBest - ind.x);
        ind.v = std::clamp(ind.v, vMin, vMax);

        ind.x += ind.v;
        ind.x = std::clamp(ind.x, left, right);

        ind.fitness = func(ind.x);
    }
}

double Particle::nonlinearDecreasingWeight(const std::uint32_t iteration) const
{
    return wBegin - ((wBegin - wEnd) * std::pow(static_cast<double>(iteration + 1) / maxIterations, 2));
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

std::optional<std::tuple<double, double>> Ant::operator()(const double left, const double right, const double eps)
{
    auto colony = colonyInit(left, right);
    double xBest = colony.front().position, yBest = func(xBest), stepLen = initialStep;

    for (std::uint32_t i = 0; i < maxIterations; ++i)
    {
        stateTransition(colony, eps);
        pathConstruction(colony, stepLen, left, right);
        updatePheromones(colony);

        const auto currBest = std::min_element(
            colony.cbegin(),
            colony.cend(),
            [](const auto& min1, const auto& min2) { return min1.pheromone < min2.pheromone; });
        if (const double x = currBest->position, y = func(x); y < yBest)
        {
            xBest = x;
            yBest = y;
        }
        stepLen = initialStep * 1.0 / (1.0 + i);
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
    const double minTau = std::min_element(
                              colony.cbegin(),
                              colony.cend(),
                              [](const auto& min1, const auto& min2) { return min1.pheromone < min2.pheromone; })
                              ->pheromone,
                 maxTau = std::max_element(
                              colony.cbegin(),
                              colony.cend(),
                              [](const auto& max1, const auto& max2) { return max1.pheromone < max2.pheromone; })
                              ->pheromone;
    std::for_each(
        colony.begin(),
        colony.end(),
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
    std::for_each(
        colony.begin(),
        colony.end(),
        [this](auto& state) { state.pheromone = (1.0 - rho) * state.pheromone + func(state.position); });
}

std::optional<std::tuple<double, double>> Genetic::operator()(const double left, const double right, const double eps)
{
    updateSpecies(left, right, eps);
    auto pop = populationInit();
    for (std::uint32_t i = 0; i < numOfGenerations; ++i)
    {
        select(pop);
        crossover(pop);
        mutate(pop);
    }
    const double xBest = geneticDecode(getBestIndividual(pop));

    return std::make_optional(std::make_tuple(func(xBest), xBest));
}

void Genetic::updateSpecies(const double left, const double right, const double eps)
{
    property.lower = left;
    property.upper = right;
    property.prec = eps;

    const double stateNum = (property.upper - property.lower) * (1.0 / property.prec);
    chromosomeLength = static_cast<std::uint32_t>(std::ceil(std::log2(stateNum)));
    if (chromosomeLength < minChrLen)
    {
        throw std::logic_error{"A precision of " + std::to_string(eps) + " is not sufficient."};
    }
}

double Genetic::geneticDecode(const Chromosome& chr) const
{
    std::uint32_t currDecoded = 0;
    std::for_each(
        chr.cbegin(),
        chr.cend(),
        [&currDecoded, index = 0](const auto bit) mutable
        { currDecoded |= static_cast<std::uint32_t>(bit) << index++; });
    const auto maxDecoded = static_cast<double>((1ULL << chromosomeLength) - 1ULL);

    return property.lower + ((property.upper - property.lower) * static_cast<double>(currDecoded) / maxDecoded);
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
            chr.reserve(chromosomeLength);
            std::generate_n(
                std::back_inserter(chr),
                chromosomeLength,
                [this, bit = std::uniform_int_distribution<std::uint8_t>(0, 1)]() mutable { return bit(engine); });
            return chr;
        });

    return pop;
}

void Genetic::geneticCross(Chromosome& chr1, Chromosome& chr2)
{
    std::uint32_t pmxBegin = 0, pmxEnd = 0;
    std::uniform_int_distribution<std::uint32_t> randomPos(0, chromosomeLength - 1);
    do
    {
        pmxBegin = randomPos(engine);
        pmxEnd = randomPos(engine);
        if (pmxBegin > pmxEnd)
        {
            std::swap(pmxBegin, pmxEnd);
        }
    }
    while ((pmxBegin == pmxEnd) || ((pmxEnd - pmxBegin) == (chromosomeLength - 1)));

    auto chrTemp = chr1;
    std::copy_n(chr2.cbegin() + pmxBegin, pmxEnd - pmxBegin, chr1.begin() + pmxBegin);
    std::copy_n(chrTemp.cbegin() + pmxBegin, pmxEnd - pmxBegin, chr2.begin() + pmxBegin);
}

void Genetic::crossover(Population& pop)
{
    std::vector<std::reference_wrapper<Chromosome>> selector(pop.begin(), pop.end());
    std::shuffle(selector.begin(), selector.end(), engine);
    for (auto chrIter = selector.cbegin(); (chrIter != selector.cend()) && (std::next(chrIter, 1) != selector.cend());
         std::advance(chrIter, 2))
    {
        if (crossPr > probability(engine))
        {
            auto &parent1 = chrIter->get(), &parent2 = std::next(chrIter, 1)->get();
            geneticCross(parent1, parent2);
        }
    }
}

void Genetic::geneticMutation(Chromosome& chr)
{
    std::for_each(
        chr.begin(),
        chr.end(),
        [this](auto& bit)
        {
            if (mutatePr > probability(engine))
            {
                bit = !bit;
            }
        });
}

void Genetic::mutate(Population& pop)
{
    std::for_each(pop.begin(), pop.end(), [this](auto& ind) { geneticMutation(ind); });
}

double Genetic::calculateFitness(const Chromosome& chr)
{
    return -func(geneticDecode(chr));
}

std::optional<std::pair<double, double>> Genetic::goldbergLinearScaling(
    const std::vector<double>& fitness, const double eps)
{
    const double fitMax = *std::max_element(fitness.cbegin(), fitness.cend()),
                 fitAvg = std::reduce(fitness.cbegin(), fitness.cend(), 0.0) / fitness.size();
    if (std::fabs(fitMax - fitAvg) < eps)
    {
        return std::nullopt;
    }
    const double alpha = (cMult - 1.0) * fitAvg / (fitMax - fitAvg),
                 beta = (fitMax - cMult * fitAvg) * fitAvg / (fitMax - fitAvg);

    return std::make_optional(std::pair<double, double>(alpha, beta));
}

auto Genetic::rouletteWheelSelection(const Population& pop, const std::vector<double>& cumFitness)
{
    const auto cumIter = std::find_if(
        cumFitness.cbegin(),
        cumFitness.cend(),
        [pr = probability(engine)](const auto cumulation) { return cumulation > pr; });
    return std::next(pop.cbegin(), std::distance(cumFitness.cbegin(), cumIter));
}

void Genetic::stochasticTournamentSelection(Population& pop, const std::vector<double>& cumFitness)
{
    Population selected{};
    selected.reserve(pop.size());
    while (selected.size() < pop.size())
    {
        auto competitor1 = *rouletteWheelSelection(pop, cumFitness),
             competitor2 = *rouletteWheelSelection(pop, cumFitness);
        selected.emplace_back(
            std::move((calculateFitness(competitor1) > calculateFitness(competitor2)) ? competitor1 : competitor2));
    }
    std::copy(selected.cbegin(), selected.cend(), pop.begin());
}

void Genetic::select(Population& pop)
{
    std::vector<double> fitness{};
    fitness.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(fitness),
        [this](const auto& ind) { return calculateFitness(ind); });
    if (const double min = *std::min_element(fitness.cbegin(), fitness.cend()); min <= 0.0)
    {
        std::for_each(fitness.begin(), fitness.end(), [min, eps = property.prec](auto& fit) { fit = fit - min + eps; });
    }

    const auto coeff =
        goldbergLinearScaling(fitness, property.prec).value_or(std::pair<double, double>(0.0, 1.0 / popSize));
    const double sum = std::accumulate(
        fitness.begin(),
        fitness.end(),
        0.0,
        [alpha = coeff.first, beta = coeff.second](const auto acc, auto& fit)
        {
            fit = alpha * fit + beta;
            return acc + fit;
        });
    std::for_each(fitness.begin(), fitness.end(), [sum](auto& fit) { fit /= sum; });
    std::partial_sum(fitness.begin(), fitness.end(), fitness.begin());

    stochasticTournamentSelection(pop, fitness);
}

Genetic::Chromosome Genetic::getBestIndividual(const Population& pop)
{
    std::vector<double> fitness{};
    fitness.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(fitness),
        [this](const auto& ind) { return calculateFitness(ind); });

    return *std::next(
        pop.cbegin(), std::distance(fitness.cbegin(), std::max_element(fitness.cbegin(), fitness.cend())));
}
} // namespace algorithm::optimal
