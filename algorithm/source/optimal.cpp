//! @file optimal.cpp
//! @author ryftchen
//! @brief The definitions (optimal) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "optimal.hpp"

#include <functional>
#include <set>
#include <stdexcept>

namespace algorithm::optimal
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version()
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::optional<std::tuple<double, double>> Gradient::operator()(const double left, const double right, const double eps)
{
    std::mt19937 engine(std::random_device{}());
    double x = 0.0, y = 0.0;
    std::uniform_real_distribution<double> candidate(left, right);
    std::set<double> climbing;
    while (climbing.size() < loopTime)
    {
        climbing.insert(candidate(engine));
    }

    std::vector<std::pair<double, double>> container;
    container.reserve(climbing.size());
    for (const auto climber : climbing)
    {
        x = climber;
        std::uint32_t numOfIteration = 0;
        double learningRate = initialLearningRate, gradient = calculateFirstDerivative(x, eps),
               dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x - dx) >= left) && ((x - dx) <= right))
        {
            x -= dx;
            ++numOfIteration;
            learningRate = initialLearningRate * 1.0 / (1.0 + decay * numOfIteration);
            gradient = calculateFirstDerivative(x, eps);
            dx = learningRate * gradient;
        }
        container.emplace_back(func(x), x);
    }

    const auto best = std::min_element(
        std::cbegin(container),
        std::cend(container),
        [](const auto& min1, const auto& min2)
        {
            return std::get<0>(min1) < std::get<0>(min2);
        });
    y = std::get<0>(*best);
    x = std::get<1>(*best);

    return std::make_optional(std::make_tuple(y, x));
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
}

std::optional<std::tuple<double, double>> Annealing::operator()(const double left, const double right, const double eps)
{
    std::uniform_real_distribution<double> perturbation(left, right);
    std::uniform_real_distribution<double> pr(0.0, 1.0);

    std::mt19937 engine(std::random_device{}());
    double temperature = initialT, x = perturbation(engine), y = func(x);
    while (temperature > minimalT)
    {
        double xBest = x, yBest = y;
        bool found = false;
        for (std::uint32_t i = 0; i < markovChain; ++i)
        {
            double xNew = static_cast<std::int64_t>(perturbation(engine) * static_cast<std::uint32_t>(1.0 / eps)) * eps,
                   yNew = func(xNew);
            if ((yNew <= y) || (std::exp(-(yNew - y) / temperature) > pr(engine)))
            {
                found = true;
                x = xNew;
                y = yNew;
                if (y < yBest)
                {
                    xBest = x;
                    yBest = y;
                }
            }
        }
        if (found)
        {
            x = xBest;
            y = yBest;
        }
        temperature *= coolingRate;
    }

    return std::make_optional(std::make_tuple(y, x));
}

std::optional<std::tuple<double, double>> Particle::operator()(const double left, const double right, const double eps)
{
    std::uniform_real_distribution<double> candidate(left, right), v(vMin, vMax);
    auto swarm = Swarm(size, Individual{});
    std::generate(
        swarm.begin(),
        swarm.end(),
        [this, &candidate, &v]()
        {
            const double x = candidate(engine);
            const Individual individual(x, v(engine), x, func(x), func(x));
            return individual;
        });
    const auto initialBest = std::min_element(
        std::cbegin(swarm),
        std::cend(swarm),
        [](const auto min1, const auto min2)
        {
            return min1.xFitness < min2.xFitness;
        });
    double xBest = initialBest->x, xFitnessBest = initialBest->xFitness;

    std::uniform_real_distribution<double> coeff(0.0, 1.0);
    std::vector<std::pair<double, double>> container;
    container.reserve(swarm.size());
    for (std::uint32_t i = 0; i < numOfIteration; ++i)
    {
        const double w = wBegin - (wBegin - wEnd) * std::pow(static_cast<double>(i + 1) / numOfIteration, 2);
        for (auto& ind : swarm)
        {
            const double rand1 =
                             static_cast<std::uint32_t>(coeff(engine) * static_cast<std::uint32_t>(1.0 / eps)) * eps,
                         rand2 =
                             static_cast<std::uint32_t>(coeff(engine) * static_cast<std::uint32_t>(1.0 / eps)) * eps;

            ind.velocity = w * ind.velocity + c1 * rand1 * (ind.positionBest - ind.x) + c2 * rand2 * (xBest - ind.x);
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
            if (ind.xFitness < ind.fitnessPositionBest)
            {
                ind.positionBest = ind.x;
                ind.fitnessPositionBest = ind.xFitness;
            }
            if (ind.xFitness < xFitnessBest)
            {
                xBest = ind.x;
                xFitnessBest = ind.xFitness;
            }
        }
        container.emplace_back(xFitnessBest, xBest);
    }

    const auto best = std::min_element(
        std::cbegin(container),
        std::cend(container),
        [](const auto& min1, const auto& min2)
        {
            return std::get<0>(min1) < std::get<0>(min2);
        });
    xFitnessBest = std::get<0>(*best);
    xBest = std::get<1>(*best);

    return std::make_optional(std::make_tuple(xFitnessBest, xBest));
}

std::optional<std::tuple<double, double>> Genetic::operator()(const double left, const double right, const double eps)
{
    updateSpecies(left, right, eps);
    if (constexpr std::uint32_t minChrNum = 3; chromosomeNum < minChrNum)
    {
        throw std::runtime_error("A precision of " + std::to_string(eps) + " is not sufficient.");
    }

    Population pop{populationInit()};
    for (std::uint32_t i = 0; i < numOfIteration; ++i)
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
    decodeAttr.lower = left;
    decodeAttr.upper = right;
    decodeAttr.eps = eps;

    std::uint32_t num = 0;
    const double max = (decodeAttr.upper - decodeAttr.lower) * (1.0 / decodeAttr.eps);
    while ((max <= std::pow(2, num)) || (max >= std::pow(2, num + 1)))
    {
        ++num;
    }
    chromosomeNum = num + 1;
}

double Genetic::geneticDecode(const Chromosome& chr) const
{
    const double max = std::pow(2, chromosomeNum) - 1.0;
    double convert = 0.0;
    std::uint32_t index = 0;
    std::for_each(
        chr.cbegin(),
        chr.cend(),
        [&convert, &index](const auto bit)
        {
            convert += bit * std::pow(2, index);
            ++index;
        });
    return decodeAttr.lower + (decodeAttr.upper - decodeAttr.lower) * convert / max;
}

Genetic::Population Genetic::populationInit()
{
    Population pop(size, Chromosome(chromosomeNum, 0));
    std::for_each(
        pop.begin(),
        pop.end(),
        [this](auto& chr)
        {
            std::uniform_int_distribution<std::uint8_t> bit(0, 1);
            std::generate(
                chr.begin(),
                chr.end(),
                [this, &bit]()
                {
                    return bit(engine);
                });
        });
    return pop;
}

void Genetic::geneticCross(Chromosome& chr1, Chromosome& chr2)
{
    Chromosome chrTemp;
    chrTemp.reserve(chr1.size());
    std::copy(chr1.cbegin(), chr1.cend(), std::back_inserter(chrTemp));

    std::uint32_t crossBegin = getRandomLessThanLimit(chromosomeNum - 1),
                  crossEnd = getRandomLessThanLimit(chromosomeNum - 1);
    if (crossBegin > crossEnd)
    {
        std::swap(crossBegin, crossEnd);
    }
    std::copy_n(chr2.cbegin() + crossBegin, crossEnd - crossBegin, chr1.begin() + crossBegin);
    std::copy_n(chrTemp.cbegin() + crossBegin, crossEnd - crossBegin, chr2.begin() + crossBegin);
}

void Genetic::crossover(Population& pop)
{
    Population popCross;
    popCross.reserve(pop.size());

    std::vector<std::reference_wrapper<Chromosome>> container(pop.begin(), pop.end());
    std::shuffle(container.begin(), container.end(), engine);
    for (auto chrIter = container.begin(); (container.end() != chrIter) && (std::next(chrIter, 1) != container.end());
         std::advance(chrIter, 2))
    {
        Chromosome parent1 = chrIter->get(), parent2 = std::next(chrIter, 1)->get();
        if (crossPr > probability())
        {
            geneticCross(parent1, parent2);
        }
        popCross.emplace_back(std::move(parent1));
        popCross.emplace_back(std::move(parent2));

        if ((pop.size() % 2) && (std::next(chrIter, 2) == container.end() - 1))
        {
            Chromosome single = std::next(chrIter, 2)->get();
            popCross.emplace_back(std::move(single));
        }
    }

    std::copy(popCross.cbegin(), popCross.cend(), pop.begin());
}

void Genetic::geneticMutation(Chromosome& chr)
{
    std::uint32_t flip = getRandomLessThanLimit(chromosomeNum - 1) + 1;
    std::vector<std::reference_wrapper<std::uint8_t>> mutateChr(chr.begin(), chr.end());
    std::shuffle(mutateChr.begin(), mutateChr.end(), engine);
    for (auto geneIter = mutateChr.begin(); (mutateChr.end() != geneIter) && (0 != flip); ++geneIter, --flip)
    {
        geneIter->get() = !geneIter->get();
    }
}

void Genetic::mutate(Population& pop)
{
    std::for_each(
        pop.begin(),
        pop.end(),
        [this](auto& ind)
        {
            if (mutatePr > probability())
            {
                geneticMutation(ind);
            }
        });
}

double Genetic::calculateFitness(const Chromosome& chr)
{
    return -func(geneticDecode(chr));
}

std::optional<std::pair<double, double>> Genetic::fitnessLinearTransformation(const Population& pop)
{
    std::vector<double> reFitness;
    reFitness.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(reFitness),
        [this](const auto& ind)
        {
            return calculateFitness(ind);
        });

    const double reFitnessMin = *(std::min_element(std::cbegin(reFitness), std::cend(reFitness))),
                 reFitnessAvg = std::accumulate(std::cbegin(reFitness), std::cend(reFitness), 0.0) / reFitness.size();
    if (std::fabs(reFitnessMin - reFitnessAvg) < decodeAttr.eps)
    {
        return std::nullopt;
    }
    const double alpha = reFitnessAvg / (reFitnessAvg - reFitnessMin),
                 beta = -(reFitnessMin * reFitnessAvg) / (reFitnessAvg - reFitnessMin);
    if (std::isnan(alpha) || std::isinf(alpha) || std::isnan(beta) || std::isinf(beta))
    {
        return std::make_optional(std::pair<double, double>(1.0, 0.0));
    }
    return std::make_optional(std::pair<double, double>(alpha, beta));
}

auto Genetic::rouletteWheelSelection(const Population& pop, const std::vector<double>& cumFitness)
{
    const double pr = probability();
    const auto cumIter = std::find_if(
        cumFitness.cbegin(),
        cumFitness.cend(),
        [&pr](const auto cumulation)
        {
            return cumulation > pr;
        });

    return std::next(pop.cbegin(), std::distance(cumFitness.cbegin(), cumIter));
}

void Genetic::stochasticTournamentSelection(Population& pop, const std::vector<double>& cumFitness)
{
    Population popNew;
    popNew.reserve(pop.size());
    while (popNew.size() < pop.size())
    {
        auto competitor1 = rouletteWheelSelection(pop, cumFitness);
        auto competitor2 = rouletteWheelSelection(pop, cumFitness);
        (calculateFitness(*competitor1) >= calculateFitness(*competitor2)) ? popNew.emplace_back(*competitor1)
                                                                           : popNew.emplace_back(*competitor2);
    }
    std::copy(popNew.cbegin(), popNew.cend(), pop.begin());
}

void Genetic::select(Population& pop)
{
    double sum = 0.0, alpha = 1.0, beta = 0.0;
    const auto coefficient = fitnessLinearTransformation(pop);
    if (coefficient.has_value())
    {
        alpha = std::get<0>(coefficient.value());
        beta = std::get<1>(coefficient.value());
    }
    std::vector<double> fitnessVal;
    fitnessVal.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(fitnessVal),
        [this, alpha, beta, &sum](const auto& ind)
        {
            const double fitVal = alpha * calculateFitness(ind) + beta;
            sum += fitVal;
            return fitVal;
        });

    std::vector<double> fitnessAvg;
    fitnessAvg.reserve(fitnessVal.size());
    std::transform(
        fitnessVal.cbegin(),
        fitnessVal.cend(),
        std::back_inserter(fitnessAvg),
        [&sum](const auto fitVal)
        {
            return fitVal / sum;
        });

    double previous = 0.0;
    std::for_each(
        fitnessAvg.begin(),
        fitnessAvg.end(),
        [&previous](auto& fitCum)
        {
            fitCum += previous;
            previous = fitCum;
        });
    stochasticTournamentSelection(pop, fitnessAvg);
}

Genetic::Chromosome Genetic::getBestIndividual(const Population& pop)
{
    std::vector<double> fitnessVal;
    fitnessVal.reserve(pop.size());
    std::transform(
        pop.cbegin(),
        pop.cend(),
        std::back_inserter(fitnessVal),
        [this](const auto& ind)
        {
            return calculateFitness(ind);
        });

    const auto fitValBestIter = std::max_element(std::cbegin(fitnessVal), std::cend(fitnessVal));
    const auto indBestIter = std::next(pop.cbegin(), std::distance(fitnessVal.cbegin(), fitValBestIter));

    return *indBestIter;
}

double Genetic::probability()
{
    std::uniform_real_distribution<double> pr(0.0, 1.0);
    return pr(engine);
}

std::uint32_t Genetic::getRandomLessThanLimit(const std::uint32_t limit)
{
    std::uniform_int_distribution<std::uint32_t> num(0, limit);
    return num(engine);
}
} // namespace algorithm::optimal
