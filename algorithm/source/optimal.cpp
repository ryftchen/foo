//! @file optimal.cpp
//! @author ryftchen
//! @brief The definitions (optimal) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "optimal.hpp"
#include <set>
#ifdef __RUNTIME_PRINTING
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

//! @brief Display optimal result.
#define OPTIMAL_RESULT(opt) "*%-9s method: F(" #opt ")=%+.5f X=%+.5f  ==>Run time: %8.5f ms\n"
//! @brief Print optimal result content.
#define OPTIMAL_PRINT_RESULT_CONTENT(method, f, x) \
    COMMON_PRINT(OPTIMAL_RESULT(min), method, f, x, OPTIMAL_RUNTIME_INTERVAL)
//! @brief Store optimal beginning runtime.
#define OPTIMAL_RUNTIME_BEGIN TIME_BEGIN(timing)
//! @brief Store optimal ending runtime.
#define OPTIMAL_RUNTIME_END TIME_END(timing)
//! @brief Calculate optimal runtime interval.
#define OPTIMAL_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else
#include <sys/time.h>
// #define NDEBUG
#include <cassert>
#include <functional>

//! @brief Print optimal result content.
#define OPTIMAL_PRINT_RESULT_CONTENT(method, f, x)
//! @brief Store optimal beginning runtime.
#define OPTIMAL_RUNTIME_BEGIN
//! @brief Store optimal ending runtime.
#define OPTIMAL_RUNTIME_END
#endif

namespace algorithm::optimal
{
//! @brief Get the random seed by time.
//! @return random seed
std::mt19937 getRandomSeedByTime()
{
    constexpr uint32_t secToUsec = 1000000;
    timeval timeSeed{};
    gettimeofday(&timeSeed, nullptr);

    return std::mt19937(timeSeed.tv_sec * secToUsec + timeSeed.tv_usec);
}

Gradient::Gradient(const function::Function& func) : func(func)
{
}

std::optional<std::tuple<ValueY, ValueX>> Gradient::operator()(const double left, const double right, const double eps)
{
    OPTIMAL_RUNTIME_BEGIN;
    std::mt19937 seed{getRandomSeedByTime()};
    double x = 0.0, y = 0.0;
    std::uniform_real_distribution<double> candidate(left, right);
    std::set<double> climbing;
    while (climbing.size() < loopTime)
    {
        climbing.insert(candidate(seed));
    }

    std::vector<std::pair<ValueY, ValueX>> aggregation;
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

    OPTIMAL_RUNTIME_END;
    OPTIMAL_PRINT_RESULT_CONTENT("Gradient", y, x);
    return std::make_optional(std::make_tuple(y, x));
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
}

Annealing::Annealing(const function::Function& func) : func(func)
{
}

std::optional<std::tuple<ValueY, ValueX>> Annealing::operator()(const double left, const double right, const double eps)
{
    OPTIMAL_RUNTIME_BEGIN;
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

    OPTIMAL_RUNTIME_END;
    OPTIMAL_PRINT_RESULT_CONTENT("Annealing", y, x);
    return std::make_optional(std::make_tuple(y, x));
}

Particle::Particle(const function::Function& func) : func(func), seed(std::random_device{}())
{
}

std::optional<std::tuple<ValueY, ValueX>> Particle::operator()(const double left, const double right, const double eps)
{
    OPTIMAL_RUNTIME_BEGIN;
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
        rec.history.insert(std::pair<ValueY, ValueX>(xFitnessBest, xBest));
    }
    xFitnessBest = std::get<0>(*(rec.history.cbegin()));
    xBest = std::get<1>(*(rec.history.cbegin()));

    OPTIMAL_RUNTIME_END;
    OPTIMAL_PRINT_RESULT_CONTENT("Particle", xFitnessBest, xBest);
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

Genetic::Genetic(const function::Function& func) : func(func), seed(std::random_device{}())
{
}

std::optional<std::tuple<ValueY, ValueX>> Genetic::operator()(const double left, const double right, const double eps)
{
    OPTIMAL_RUNTIME_BEGIN;
    constexpr uint32_t minChrNum = 3;
    updateSpecies(left, right, eps);
    if (chrNum < minChrNum)
    {
#ifdef __RUNTIME_PRINTING
        COMMON_PRINT("*Genetic   method: A precision of %.5f isn't sufficient.\n", eps);
#endif
        return std::nullopt;
    }

    Population pop = populationInit();
    for (uint32_t i = 0; i < numOfIteration; ++i)
    {
        select(pop);
        crossover(pop);
        mutate(pop);
    }
    const double x = geneticDecode(getBestIndividual(pop));

    OPTIMAL_RUNTIME_END;
    OPTIMAL_PRINT_RESULT_CONTENT("Genetic", func(x), x);
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
