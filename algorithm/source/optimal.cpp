#include "optimal.hpp"
#include <set>
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

#define OPTIMAL_RESULT(opt) "*%-9s method: F(" #opt ")=%+.5f X=%+.5f  ==>Run time: %8.5f ms\n"

namespace num_optimal
{
// Gradient Descent
std::optional<std::tuple<ValueY, ValueX>> Gradient::operator()(const double left, const double right, const double eps)
{
    TIME_BEGIN(timing);
    std::mt19937 seed{util_time::getRandomSeedByTime()};
    double x = 0.0, y = 0.0;
    std::uniform_real_distribution<double> randomX(left, right);
    std::set<double> climbing;
    while (climbing.size() < gradient::loopTime)
    {
        climbing.insert(randomX(seed));
    }

    std::vector<std::pair<ValueY, ValueX>> aggregation;
    aggregation.reserve(climbing.size());
    for (const auto climber : climbing)
    {
        x = climber;
        uint32_t numOfIteration = 0;
        double learningRate = gradient::initialLearningRate, gradient = calculateFirstDerivative(x, eps),
               dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x - dx) >= left) && ((x - dx) <= right))
        {
            x -= dx;
            ++numOfIteration;
            learningRate = gradient::initialLearningRate * 1.0 / (1.0 + gradient::decay * numOfIteration);
            gradient = calculateFirstDerivative(x, eps);
            dx = learningRate * gradient;
        }
        aggregation.emplace_back(std::pair<ValueY, ValueX>(func(x), x));
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

    TIME_END(timing);
    COMMON_PRINT(OPTIMAL_RESULT(min), "Gradient", y, x, TIME_INTERVAL(timing));
    return std::make_optional(std::make_tuple(y, x));
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
}

// Simulated Annealing
std::optional<std::tuple<ValueY, ValueX>> Annealing::operator()(const double left, const double right, const double eps)
{
    TIME_BEGIN(timing);
    constexpr double perturbation = 0.5;
    double temperature = annealing::initialT;
    std::mt19937 seed{util_time::getRandomSeedByTime()};
    std::uniform_real_distribution<double> randomX(left, right);
    std::uniform_real_distribution<double> random(-perturbation, perturbation);
    double x = randomX(seed), y = func(x);
    while (temperature > annealing::minimalT)
    {
        double xBest = x, yBest = y;
        bool found = false;
        for (uint32_t i = 0; i < annealing::markovChain; ++i)
        {
            double xNew = 0.0, yNew = 0.0;
            do
            {
                double delta = static_cast<int>(random(seed) * (right - left) * static_cast<uint32_t>(1.0 / eps)) * eps;
                xNew = x + delta;
            }
            while ((xNew < left) || (xNew > right));
            yNew = func(xNew);

            if ((yNew < y) || (std::exp(-(y - yNew) / temperature) > random(seed)))
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
        temperature *= annealing::coolingRate;
    }

    TIME_END(timing);
    COMMON_PRINT(OPTIMAL_RESULT(min), "Annealing", y, x, TIME_INTERVAL(timing));
    return std::make_optional(std::make_tuple(y, x));
}

// Particle Swarm
std::optional<std::tuple<ValueY, ValueX>> Particle::operator()(const double left, const double right, const double eps)
{
    TIME_BEGIN(timing);
    Record rec = recordInit(left, right);
    const auto best = std::min_element(
        std::cbegin(rec.society),
        std::cend(rec.society),
        [](const auto min1, const auto min2)
        {
            return min1.xFitness < min2.xFitness;
        });
    double xBest = best->x, xFitnessBest = best->xFitness;

    std::uniform_real_distribution<double> random(0.0, 1.0);
    for (uint32_t i = 0; i < particle::numOfIteration; ++i)
    {
        const double w = particle::wBegin
            - (particle::wBegin - particle::wEnd) * std::pow(static_cast<double>(i + 1) / particle::numOfIteration, 2);
        for (auto& ind : rec.society)
        {
            const double rand1 = static_cast<uint32_t>(random(seed) * static_cast<uint32_t>(1.0 / eps)) * eps;
            const double rand2 = static_cast<uint32_t>(random(seed) * static_cast<uint32_t>(1.0 / eps)) * eps;
            ind.velocity = w * ind.velocity + particle::c1 * rand1 * (ind.positionBest - ind.x)
                + particle::c2 * rand2 * (xBest - ind.x);
            (ind.velocity > particle::vMax)
                ? ind.velocity = particle::vMax
                : ((ind.velocity < particle::vMin) ? ind.velocity = particle::vMin : ind.velocity);

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

    TIME_END(timing);
    COMMON_PRINT(OPTIMAL_RESULT(min), "Particle", xFitnessBest, xBest, TIME_INTERVAL(timing));
    return std::make_optional(std::make_tuple(xFitnessBest, xBest));
}

particle::Record Particle::recordInit(const double left, const double right)
{
    seed = std::mt19937{util_time::getRandomSeedByTime()};
    std::uniform_real_distribution<double> randomX(left, right), randomV(particle::vMin, particle::vMax);

    const Individual individualInit{};
    Society societyInit(particle::size, individualInit);
    std::generate(
        societyInit.begin(),
        societyInit.end(),
        [&]
        {
            const double x = randomX(seed);
            const Individual individual(x, randomV(seed), x, func(x), func(x));
            return individual;
        });
    Record rec{{}, {}};
    rec.society = societyInit;
    return rec;
}

// Genetic
std::optional<std::tuple<ValueY, ValueX>> Genetic::operator()(const double left, const double right, const double eps)
{
    TIME_BEGIN(timing);
    constexpr uint32_t minChrNum = 3;
    updateSpecies(left, right, eps);
    if (chrNum < minChrNum)
    {
        COMMON_PRINT("*Genetic   method: The precise %.5f isn't enough.\n", eps);
        return std::nullopt;
    }

    Population pop = populationInit();
    for (uint32_t i = 0; i < genetic::numOfIteration; ++i)
    {
        selectIndividual(pop);
        crossIndividual(pop);
        mutateIndividual(pop);
    }
    const double x = geneDecoding(getBestIndividual(pop));

    TIME_END(timing);
    COMMON_PRINT(OPTIMAL_RESULT(min), "Genetic", func(x), x, TIME_INTERVAL(timing));
    return std::make_optional(std::make_tuple(func(x), x));
}

void Genetic::updateSpecies(const double left, const double right, const double eps)
{
    range.lower = left;
    range.upper = right;
    range.eps = eps;
    seed = std::mt19937{util_time::getRandomSeedByTime()};

    uint32_t num = 0;
    const double max = (range.upper - range.lower) * (1.0 / range.eps);
    while ((max <= std::pow(2, num)) || (max >= std::pow(2, num + 1)))
    {
        ++num;
    }
    chrNum = num + 1;
}

void Genetic::geneCoding(Chromosome& chr)
{
    std::uniform_int_distribution<int> randomX(0, 1);
    std::generate(
        chr.begin(),
        chr.end(),
        [&]
        {
            return static_cast<uint8_t>(randomX(seed));
        });
}

double Genetic::geneDecoding(const Chromosome& chr) const
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

genetic::Population Genetic::populationInit()
{
    const Chromosome chrInit(chrNum, 0);
    Population pop(genetic::size, chrInit);
    std::for_each(
        pop.begin(),
        pop.end(),
        [this](auto& chr)
        {
            geneCoding(chr);
        });
    return pop;
}

void Genetic::geneCrossover(Chromosome& chr1, Chromosome& chr2)
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

void Genetic::crossIndividual(Population& pop)
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
        if (genetic::crossPr > random())
        {
            geneCrossover(parent1, parent2);
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

void Genetic::geneMutation(Chromosome& chr)
{
    uint32_t flip = getRandomNumber(chrNum - 1) + 1;
    std::vector<std::reference_wrapper<uint8_t>> mutateChr(chr.begin(), chr.end());
    std::shuffle(mutateChr.begin(), mutateChr.end(), seed);
    for (auto geneIter = mutateChr.begin(); (mutateChr.end() != geneIter) && (0 != flip); ++geneIter, --flip)
    {
        geneIter->get() = !geneIter->get();
    }
}

void Genetic::mutateIndividual(Population& pop)
{
    std::for_each(
        pop.begin(),
        pop.end(),
        [this](auto& ind)
        {
            if (genetic::mutatePr > random())
            {
                geneMutation(ind);
            }
        });
}

double Genetic::calculateFitness(const Chromosome& chr)
{
    return -func(geneDecoding(chr));
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

auto Genetic::rouletteWheelSelection(const Population& pop, const std::vector<double>& fitnessCum)
{
    const double rand = random();
    const auto cumIter = std::find_if(
        fitnessCum.cbegin(),
        fitnessCum.cend(),
        [&rand](const auto cumulation)
        {
            return cumulation > rand;
        });
    assert(cumIter != fitnessCum.cend());

    return std::next(pop.cbegin(), std::distance(fitnessCum.cbegin(), cumIter));
}

void Genetic::stochasticTournamentSelection(Population& pop, const std::vector<double>& fitnessCum)
{
    Population popNew;
    popNew.reserve(pop.size());
    while (popNew.size() < pop.size())
    {
        auto competitor1 = rouletteWheelSelection(pop, fitnessCum);
        auto competitor2 = rouletteWheelSelection(pop, fitnessCum);
        (calculateFitness(*competitor1) >= calculateFitness(*competitor2)) ? popNew.emplace_back(*competitor1)
                                                                           : popNew.emplace_back(*competitor2);
    }
    std::copy(popNew.cbegin(), popNew.cend(), pop.begin());
}

void Genetic::selectIndividual(Population& pop)
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

genetic::Chromosome Genetic::getBestIndividual(const Population& pop)
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
} // namespace num_optimal
