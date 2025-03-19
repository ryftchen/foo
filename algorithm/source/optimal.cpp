//! @file optimal.cpp
//! @author ryftchen
//! @brief The definitions (optimal) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "optimal.hpp"

#include <functional>
#include <stdexcept>
#include <unordered_set>

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
    std::mt19937_64 engine(std::random_device{}());
    std::uniform_real_distribution<double> candidate(left, right);
    std::unordered_multiset<double> climbing{};
    climbing.reserve(loopTime);
    while (climbing.size() < loopTime)
    {
        climbing.emplace(candidate(engine));
    }

    double x = 0.0, y = 0.0, xBest = x, yBest = y;
    for (const auto climber : climbing)
    {
        x = climber;
        std::uint32_t iteration = 0;
        double learningRate = initialLearningRate, gradient = calculateFirstDerivative(x, eps),
               dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x - dx) >= left) && ((x - dx) <= right))
        {
            x -= dx;
            ++iteration;
            learningRate = initialLearningRate * 1.0 / (1.0 + decay * iteration);
            gradient = calculateFirstDerivative(x, eps);
            dx = learningRate * gradient;
        }

        y = func(x);
        if (y < yBest)
        {
            xBest = x;
            yBest = y;
        }
    }

    return std::make_optional(std::make_tuple(yBest, xBest));
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
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
        for (std::uint32_t i = 0; i < markovChain; ++i)
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
    return prev + (temp * sgn * (1 + 1 / (std::pow(temp, 2 * xi - 1) - 1))) * (max - min);
}

bool Annealing::metropolisAcceptanceCriterion(const double deltaE, const double temp, const double xi)
{
    return (deltaE < 0) || (std::exp(-deltaE / temp) > xi);
}

std::optional<std::tuple<double, double>> Particle::operator()(const double left, const double right, const double eps)
{
    auto swarm{swarmInit(left, right)};
    const auto initialBest = std::min_element(
        swarm.cbegin(), swarm.cend(), [](const auto& min1, const auto& min2) { return min1.xFitness < min2.xFitness; });
    double gloBest = initialBest->x, gloBestFitness = initialBest->xFitness;

    std::uniform_real_distribution<double> coeff(0.0, 1.0);
    for (std::uint32_t i = 0; i < maxIterations; ++i)
    {
        for (const double w = nonlinearDecreasingWeight(i); auto& ind : swarm)
        {
            const double rand1 = std::round(coeff(engine) / eps) * eps, rand2 = std::round(coeff(engine) / eps) * eps;
            ind.velocity = w * ind.velocity + c1 * rand1 * (ind.posBest - ind.x) + c2 * rand2 * (gloBest - ind.x);
            if (ind.velocity > vMax)
            {
                ind.velocity = vMax;
            }
            else if (ind.velocity < vMin)
            {
                ind.velocity = vMin;
            }

            ind.x += ind.velocity;
            if (ind.x > right)
            {
                ind.x = right;
            }
            else if (ind.x < left)
            {
                ind.x = left;
            }
            ind.xFitness = func(ind.x);
        }

        for (auto& ind : swarm)
        {
            if (ind.xFitness < ind.posBestFitness)
            {
                ind.posBest = ind.x;
                ind.posBestFitness = ind.xFitness;
            }
            if (ind.xFitness < gloBestFitness)
            {
                gloBest = ind.x;
                gloBestFitness = ind.xFitness;
            }
        }
    }

    return std::make_optional(std::make_tuple(gloBestFitness, gloBest));
}

Particle::Swarm Particle::swarmInit(const double left, const double right)
{
    std::uniform_real_distribution<double> candidate(left, right), vel(vMin, vMax);
    Swarm swarm(size, Individual{});
    std::generate(
        swarm.begin(),
        swarm.end(),
        [this, &candidate, &vel]()
        {
            const double x = candidate(engine);
            return Individual{x, vel(engine), x, func(x), func(x)};
        });

    return swarm;
}

double Particle::nonlinearDecreasingWeight(const std::uint32_t iteration)
{
    return wBegin - (wBegin - wEnd) * std::pow(static_cast<double>(iteration + 1) / maxIterations, 2);
}

std::optional<std::tuple<double, double>> Genetic::operator()(const double left, const double right, const double eps)
{
    updateSpecies(left, right, eps);
    auto pop{populationInit()};
    for (std::uint32_t i = 0; i < numOfGeneration; ++i)
    {
        select(pop);
        crossover(pop);
        mutate(pop);
    }
    const double x = geneticDecode(getBestIndividual(pop));

    return std::make_optional(std::make_tuple(func(x), x));
}

void Genetic::updateSpecies(const double left, const double right, const double eps)
{
    property.lower = left;
    property.upper = right;
    property.eps = eps;

    std::uint32_t num = 0;
    const double max = (property.upper - property.lower) * (1.0 / property.eps);
    while ((max <= std::pow(2, num)) || (max >= std::pow(2, num + 1)))
    {
        ++num;
    }
    chromosomeNum = num + 1;

    if (constexpr std::uint32_t minChrNum = 2; chromosomeNum < minChrNum)
    {
        throw std::logic_error{"A precision of " + std::to_string(eps) + " is not sufficient."};
    }
}

double Genetic::geneticDecode(const Chromosome& chr) const
{
    const double max = std::pow(2, chromosomeNum) - 1.0;
    double convert = 0.0;
    std::for_each(
        chr.cbegin(),
        chr.cend(),
        [&convert, index = 0](const auto bit) mutable
        {
            convert += bit * std::pow(2, index);
            ++index;
        });

    return property.lower + (property.upper - property.lower) * convert / max;
}

Genetic::Population Genetic::populationInit()
{
    Population pop(size, Chromosome(chromosomeNum, 0));
    std::for_each(
        pop.begin(),
        pop.end(),
        [this, bit = std::uniform_int_distribution<std::uint8_t>(0, 1)](auto& chr) mutable
        { std::generate(chr.begin(), chr.end(), [this, &bit]() { return bit(engine); }); });

    return pop;
}

void Genetic::geneticCross(Chromosome& chr1, Chromosome& chr2)
{
    std::uint32_t pmxBegin = 0, pmxEnd = 0;
    std::uniform_int_distribution<std::uint32_t> randomPos(0, chromosomeNum - 1);
    do
    {
        pmxBegin = randomPos(engine);
        pmxEnd = randomPos(engine);
        if (pmxBegin > pmxEnd)
        {
            std::swap(pmxBegin, pmxEnd);
        }
    }
    while ((pmxBegin == pmxEnd) || ((pmxEnd - pmxBegin) == (chromosomeNum - 1)));

    auto chrTemp{chr1};
    std::copy_n(chr2.cbegin() + pmxBegin, pmxEnd - pmxBegin, chr1.begin() + pmxBegin);
    std::copy_n(chrTemp.cbegin() + pmxBegin, pmxEnd - pmxBegin, chr2.begin() + pmxBegin);
}

void Genetic::crossover(Population& pop)
{
    std::vector<std::reference_wrapper<Chromosome>> selector(pop.begin(), pop.end());
    std::shuffle(selector.begin(), selector.end(), engine);
    for (auto chrIter = selector.begin(); (selector.end() != chrIter) && (selector.end() != std::next(chrIter, 1));
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

std::optional<std::pair<double, double>> Genetic::fitnessLinearTransformation(const Population& pop)
{
    std::vector<double> reFitness{};
    reFitness.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(reFitness),
        [this](const auto& ind) { return calculateFitness(ind); });

    const double reFitnessMin = *std::min_element(reFitness.cbegin(), reFitness.cend()),
                 reFitnessAvg = std::reduce(reFitness.cbegin(), reFitness.cend(), 0.0) / reFitness.size();
    if (std::fabs(reFitnessMin - reFitnessAvg) < property.eps)
    {
        return std::nullopt;
    }
    const double alpha = reFitnessAvg / (reFitnessAvg - reFitnessMin),
                 beta = -(reFitnessMin * reFitnessAvg) / (reFitnessAvg - reFitnessMin);
    if (std::isnan(alpha) || std::isinf(alpha) || std::isnan(beta) || std::isinf(beta))
    {
        return std::nullopt;
    }

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
            std::move((calculateFitness(competitor1) >= calculateFitness(competitor2)) ? competitor1 : competitor2));
    }
    std::copy(selected.cbegin(), selected.cend(), pop.begin());
}

void Genetic::select(Population& pop)
{
    const auto coeff = fitnessLinearTransformation(pop).value_or(std::pair<double, double>(1.0, 0.0));
    double sum = 0.0;
    std::vector<double> fitnessVal{};
    fitnessVal.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(fitnessVal),
        [this, alpha = coeff.first, beta = coeff.second, &sum](const auto& ind)
        {
            const double fitVal = alpha * calculateFitness(ind) + beta;
            sum += fitVal;
            return fitVal;
        });

    std::vector<double> fitnessAvg{};
    fitnessAvg.reserve(fitnessVal.size());
    std::transform(
        fitnessVal.cbegin(),
        fitnessVal.cend(),
        std::back_inserter(fitnessAvg),
        [sum](const auto fitVal) { return fitVal / sum; });

    std::for_each(
        fitnessAvg.begin(),
        fitnessAvg.end(),
        [previous = 0.0](auto& fitCum) mutable
        {
            fitCum += previous;
            previous = fitCum;
        });
    stochasticTournamentSelection(pop, fitnessAvg);
}

Genetic::Chromosome Genetic::getBestIndividual(const Population& pop)
{
    std::vector<double> fitnessVal{};
    fitnessVal.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(fitnessVal),
        [this](const auto& ind) { return calculateFitness(ind); });

    const auto fitValBestIter = std::max_element(fitnessVal.cbegin(), fitnessVal.cend());
    const auto indBestIter = std::next(pop.cbegin(), std::distance(fitnessVal.cbegin(), fitValBestIter));

    return *indBestIter;
}
} // namespace algorithm::optimal
