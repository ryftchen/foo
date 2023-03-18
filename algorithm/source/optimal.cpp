//! @file optimal.cpp
//! @author ryftchen
//! @brief The definitions (optimal) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "optimal.hpp"
// #define NDEBUG
#include <cassert>
#include <chrono>
#include <functional>
#include <set>
#include <stdexcept>

namespace algorithm::optimal
{
//! @brief Get the random seed by time.
//! @return random seed
std::mt19937 getRandomSeedByTime()
{
    const auto now = std::chrono::system_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return std::mt19937(milliseconds);
}

std::optional<std::tuple<double, double>> Gradient::operator()(const double left, const double right, const double eps)
{
    std::mt19937 seed{getRandomSeedByTime()};
    double x = 0.0, y = 0.0;
    std::uniform_real_distribution<double> candidate(left, right);
    std::set<double> climbing;
    while (climbing.size() < loopTime)
    {
        climbing.insert(candidate(seed));
    }

    std::vector<std::pair<double, double>> aggregation;
    aggregation.reserve(climbing.size());
    for (const auto climber : climbing)
    {
        x = climber;
        uint32_t numOfIteration = 0;
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
        aggregation.emplace_back(func(x), x);
    }

    const auto best = std::min_element(
        std::cbegin(aggregation),
        std::cend(aggregation),
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

    std::mt19937 seed{getRandomSeedByTime()};
    double temperature = initialT, x = perturbation(seed), y = func(x);
    while (temperature > minimalT)
    {
        double xBest = x, yBest = y;
        bool found = false;
        for (uint32_t i = 0; i < markovChain; ++i)
        {
            double xNew = static_cast<int>(perturbation(seed) * static_cast<uint32_t>(1.0 / eps)) * eps,
                   yNew = func(xNew);
            if ((yNew <= y) || (std::exp(-(yNew - y) / temperature) > pr(seed)))
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
    Storage rec = storageInit(left, right);
    const auto best = std::min_element(
        std::cbegin(rec.society),
        std::cend(rec.society),
        [](const auto min1, const auto min2)
        {
            return min1.xFitness < min2.xFitness;
        });
    double xBest = best->x, xFitnessBest = best->xFitness;

    std::uniform_real_distribution<double> random(0.0, 1.0);
    for (uint32_t i = 0; i < numOfIteration; ++i)
    {
        const double w = wBegin - (wBegin - wEnd) * std::pow(static_cast<double>(i + 1) / numOfIteration, 2);
        for (auto& ind : rec.society)
        {
            const double rand1 = static_cast<uint32_t>(random(seed) * static_cast<uint32_t>(1.0 / eps)) * eps;
            const double rand2 = static_cast<uint32_t>(random(seed) * static_cast<uint32_t>(1.0 / eps)) * eps;
            ind.velocity = w * ind.velocity + c1 * rand1 * (ind.positionBest - ind.x) + c2 * rand2 * (xBest - ind.x);
            (ind.velocity > vMax) ? ind.velocity = vMax : ((ind.velocity < vMin) ? ind.velocity = vMin : ind.velocity);

            ind.x += ind.velocity;
            (ind.x > right) ? ind.x = right : ((ind.x < left) ? ind.x = left : ind.x);
            ind.xFitness = func(ind.x);
        }

        for (auto& ind : rec.society)
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
        rec.history.insert(std::pair<double, double>(xFitnessBest, xBest));
    }
    xFitnessBest = std::get<0>(*(rec.history.cbegin()));
    xBest = std::get<1>(*(rec.history.cbegin()));

    return std::make_optional(std::make_tuple(xFitnessBest, xBest));
}

Particle::Storage Particle::storageInit(const double left, const double right)
{
    seed = std::mt19937{getRandomSeedByTime()};
    std::uniform_real_distribution<double> candidate(left, right), randomV(vMin, vMax);

    const Individual individualInit{};
    Society societyInit(size, individualInit);
    std::generate(
        societyInit.begin(),
        societyInit.end(),
        [&]
        {
            const double x = candidate(seed);
            const Individual individual(x, randomV(seed), x, func(x), func(x));
            return individual;
        });
    Storage rec{{}, {}};
    rec.society = societyInit;
    return rec;
}

std::optional<std::tuple<double, double>> Genetic::operator()(const double left, const double right, const double eps)
{
    updateSpecies(left, right, eps);
    if (constexpr uint32_t minChrNum = 3; chrNum < minChrNum)
    {
        throw std::runtime_error("A precision of " + std::to_string(eps) + " isn't sufficient.");
    }

    Population pop = populationInit();
    for (uint32_t i = 0; i < numOfIteration; ++i)
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
    range.lower = left;
    range.upper = right;
    range.eps = eps;
    seed = std::mt19937{getRandomSeedByTime()};

    uint32_t num = 0;
    const double max = (range.upper - range.lower) * (1.0 / range.eps);
    while ((max <= std::pow(2, num)) || (max >= std::pow(2, num + 1)))
    {
        ++num;
    }
    chrNum = num + 1;
}

void Genetic::geneticCode(Chromosome& chr)
{
    std::uniform_int_distribution<int> random(0, 1);
    std::generate(
        chr.begin(),
        chr.end(),
        [&]
        {
            return static_cast<uint8_t>(random(seed));
        });
}

double Genetic::geneticDecode(const Chromosome& chr) const
{
    const double max = std::pow(2, chrNum) - 1.0;
    double temp = 0.0;
    uint32_t i = 0;
    std::for_each(
        chr.cbegin(),
        chr.cend(),
        [&temp, &i](const auto bit)
        {
            temp += bit * std::pow(2, i);
            ++i;
        });
    return range.lower + (range.upper - range.lower) * temp / max;
}

Genetic::Population Genetic::populationInit()
{
    const Chromosome chrInit(chrNum, 0);
    Population pop(size, chrInit);
    std::for_each(
        pop.begin(),
        pop.end(),
        [this](auto& chr)
        {
            geneticCode(chr);
        });
    return pop;
}

void Genetic::geneticCross(Chromosome& chr1, Chromosome& chr2)
{
    Chromosome chrTemp;
    chrTemp.reserve(chr1.size());
    std::copy(chr1.cbegin(), chr1.cend(), std::back_inserter(chrTemp));

    uint32_t crossBegin = getRandomNumber(chrNum - 1), crossEnd = getRandomNumber(chrNum - 1);
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

    std::vector<std::reference_wrapper<Chromosome>> crossContainer(pop.begin(), pop.end());
    std::shuffle(crossContainer.begin(), crossContainer.end(), seed);
    for (auto chrIter = crossContainer.begin();
         (crossContainer.end() != chrIter) && (std::next(chrIter, 1) != crossContainer.end());
         std::advance(chrIter, 2))
    {
        Chromosome parent1 = chrIter->get(), parent2 = std::next(chrIter, 1)->get();
        if (crossPr > probability())
        {
            geneticCross(parent1, parent2);
        }
        popCross.emplace_back(std::move(parent1));
        popCross.emplace_back(std::move(parent2));

        if ((pop.size() % 2) && (std::next(chrIter, 2) == crossContainer.end() - 1))
        {
            Chromosome single = std::next(chrIter, 2)->get();
            popCross.emplace_back(std::move(single));
        }
    }

    std::copy(popCross.cbegin(), popCross.cend(), pop.begin());
}

void Genetic::geneticMutation(Chromosome& chr)
{
    uint32_t flip = getRandomNumber(chrNum - 1) + 1;
    std::vector<std::reference_wrapper<uint8_t>> mutateChr(chr.begin(), chr.end());
    std::shuffle(mutateChr.begin(), mutateChr.end(), seed);
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
    if (std::fabs(reFitnessMin - reFitnessAvg) < (range.eps * range.eps))
    {
        return std::nullopt;
    }
    const double alpha = reFitnessAvg / (reFitnessAvg - reFitnessMin),
                 beta = -(reFitnessMin * reFitnessAvg) / (reFitnessAvg - reFitnessMin);
    assert(!std::isnan(alpha) && !std::isinf(alpha) && !std::isnan(beta) && !std::isinf(beta));
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
    assert(cumIter != cumFitness.cend());

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
        [&](const auto& ind)
        {
            const double fitVal = alpha * calculateFitness(ind) + beta;
            sum += fitVal;
            return fitVal;
        });
    assert(0 != sum);

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
} // namespace algorithm::optimal
