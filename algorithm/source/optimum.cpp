#include "optimum.hpp"
#include <algorithm>
#include <set>
#include "file.hpp"
#include "time.hpp"

using Species::Chromosome;
using Species::Population;
using Swarm::Individual;
using Swarm::Record;
using Swarm::Society;

// Fibonacci method
std::optional<std::tuple<ValueY, ValueX>> Fibonacci::operator()(
    const double left, const double right, const double eps)
{
    TIMER_BEGIN;
    double leftVal = left, rightVal = right;
    std::vector<double> fibonacci(0);
    generateFibonacciNumber(fibonacci, (rightVal - leftVal) / eps);
    int n = fibonacci.size() - 1;
    if (n < OPTIMUM_FIBONACCI_MIN_COUNT)
    {
        FORMAT_PRINT("*Fibonacci method: The precise %.5f isn't enough.\n", eps);
        return std::nullopt;
    }

    double x1 = OPTIMUM_FIBONACCI_X_1, x2 = OPTIMUM_FIBONACCI_X_2;
    while (n > OPTIMUM_FIBONACCI_MIN_COUNT)
    {
        if (func(x1) < func(x2))
        {
            leftVal = x1;
            x1 = x2;
            x2 = OPTIMUM_FIBONACCI_X_2;
        }
        else if (func(x1) > func(x2))
        {
            rightVal = x2;
            x2 = x1;
            x1 = OPTIMUM_FIBONACCI_X_1;
        }
        else
        {
            leftVal = x1;
            rightVal = x2;
            x1 = OPTIMUM_FIBONACCI_X_1;
            x2 = OPTIMUM_FIBONACCI_X_2;
        }
        --n;
    }

    x1 = leftVal + fibonacci[1] / fibonacci[2] * (rightVal - leftVal);
    x2 = x1 + (((x1 + eps) < right) ? eps : -eps);
    if (func(x1) < func(x2))
    {
        leftVal = x1;
    }
    else if (func(x1) > func(x2))
    {
        rightVal = x2;
    }
    else
    {
        leftVal = x1;
        rightVal = x2;
    }
    const double x = (leftVal + rightVal) / 2.0;

    TIMER_END;
    FORMAT_PRINT(OPTIMUM_RESULT(max), "Fibonacci", func(x), x, TIMER_INTERVAL);
    return std::make_optional(std::make_tuple(func(x), x));
}

void Fibonacci::generateFibonacciNumber(std::vector<double>& fibonacci, const double max)
{
    double f1 = 0.0, f2 = 1.0;
    for (;;)
    {
        double temp = f1 + f2;
        f1 = f2;
        f2 = temp;
        fibonacci.emplace_back(f1);

        if (f1 > max)
        {
            break;
        }
    }
}

// Gradient ascent method
std::optional<std::tuple<ValueY, ValueX>> Gradient::operator()(
    const double left, const double right, const double eps)
{
    TIMER_BEGIN;
    TIME_GET_SEED(seed);
    double x = 0.0, y = 0.0;
    std::uniform_real_distribution<double> randomX(left, right);
    std::set<double> climbing;
    while (climbing.size() < Learning::loopTime)
    {
        climbing.insert(randomX(seed));
    }

    std::vector<std::pair<ValueY, ValueX>> aggregation;
    aggregation.reserve(climbing.size());
    for (const auto& climber : climbing)
    {
        x = climber;
        uint32_t iterNum = 0;
        double learningRate = Learning::initialLearningRate;
        double gradient = calculateFirstDerivative(x, eps);
        double dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x + dx) >= left) && ((x + dx) <= right))
        {
            x += dx;
            ++iterNum;
            learningRate = Learning::initialLearningRate * 1.0 / (1.0 + Learning::decay * iterNum);
            gradient = calculateFirstDerivative(x, eps);
            dx = learningRate * gradient;
        }
        aggregation.emplace_back(std::pair<ValueY, ValueX>(func(x), x));
    }

    const auto best = std::max_element(
        std::cbegin(aggregation), std::cend(aggregation),
        [](const auto& max1, const auto& max2)
        {
            return std::get<0>(max1) < std::get<0>(max2);
        });
    y = std::get<0>(*best);
    x = std::get<1>(*best);

    TIMER_END;
    FORMAT_PRINT(OPTIMUM_RESULT(max), "Gradient", y, x, TIMER_INTERVAL);
    return std::make_optional(std::make_tuple(y, x));
}

double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (func(x + differential) - func(x - differential)) / eps;
}

// Simulated annealing method
std::optional<std::tuple<ValueY, ValueX>> Annealing::operator()(
    const double left, const double right, const double eps)
{
    TIMER_BEGIN;
    double temperature = Cooling::initialT;
    TIME_GET_SEED(seed);
    std::uniform_real_distribution<double> randomX(left, right);
    std::uniform_real_distribution<double> random(
        -OPTIMUM_ANNEALING_PERTURBATION, OPTIMUM_ANNEALING_PERTURBATION);
    double x = randomX(seed);
    double y = func(x);
    while (temperature > Cooling::minimalT)
    {
        double xBest = x;
        double yBest = y;
        bool found = false;
        for (uint32_t i = 0; i < Cooling::markovChain; ++i)
        {
            double xNew = 0.0, yNew = 0.0;
            do
            {
                double delta = static_cast<int>(
                                   random(seed) * (right - left) * static_cast<uint32_t>(1.0 / eps))
                    * eps;
                xNew = x + delta;
            }
            while ((xNew < left) || (xNew > right));
            yNew = func(xNew);

            if ((yNew > y) || (std::exp(-(y - yNew) / temperature) > random(seed)))
            {
                found = true;
                x = xNew;
                y = yNew;
                if (y > yBest)
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
        temperature *= Cooling::coolingRate;
    }

    TIMER_END;
    FORMAT_PRINT(OPTIMUM_RESULT(max), "Annealing", y, x, TIMER_INTERVAL);
    return std::make_optional(std::make_tuple(y, x));
}

// Particle swarm method
std::optional<std::tuple<ValueY, ValueX>> Particle::operator()(
    const double left, const double right, const double eps)
{
    TIMER_BEGIN;
    Record rec = recordInit(left, right);
    const auto best = std::max_element(
        std::cbegin(rec.society), std::cend(rec.society),
        [](const auto& max1, const auto& max2)
        {
            return max1.xFitness < max2.xFitness;
        });
    double xBest = best->x;
    double xFitnessBest = best->xFitness;

    std::uniform_real_distribution<double> random(0.0, 1.0);
    for (uint32_t i = 0; i < Swarm::iterNum; ++i)
    {
        const double w = Swarm::wBegin
            - (Swarm::wBegin - Swarm::wEnd)
                * std::pow(static_cast<double>(i + 1) / Swarm::iterNum, 2);
        for (auto& ind : rec.society)
        {
            const double rand1 =
                static_cast<uint32_t>(random(seed) * static_cast<uint32_t>(1.0 / eps)) * eps;
            const double rand2 =
                static_cast<uint32_t>(random(seed) * static_cast<uint32_t>(1.0 / eps)) * eps;
            ind.velocity = w * ind.velocity + Swarm::c1 * rand1 * (ind.positionBest - ind.x)
                + Swarm::c2 * rand2 * (xBest - ind.x);
            (ind.velocity > Swarm::vMax)
                ? ind.velocity = Swarm::vMax
                : ((ind.velocity < Swarm::vMin) ? ind.velocity = Swarm::vMin : ind.velocity);

            ind.x += ind.velocity;
            (ind.x > right) ? ind.x = right : ((ind.x < left) ? ind.x = left : ind.x);
            ind.xFitness = func(ind.x);
        }

        for (auto& ind : rec.society)
        {
            if (ind.xFitness > ind.fitnessPositionBest)
            {
                ind.positionBest = ind.x;
                ind.fitnessPositionBest = ind.xFitness;
            }
            if (ind.xFitness > xFitnessBest)
            {
                xBest = ind.x;
                xFitnessBest = ind.xFitness;
            }
        }
        rec.history.insert(std::pair<ValueY, ValueX>(xFitnessBest, xBest));
    }
    xFitnessBest = std::get<0>(*(rec.history.cbegin()));
    xBest = std::get<1>(*(rec.history.cbegin()));

    TIMER_END;
    FORMAT_PRINT(OPTIMUM_RESULT(max), "Particle", xFitnessBest, xBest, TIMER_INTERVAL);
    return std::make_optional(std::make_tuple(xFitnessBest, xBest));
}

Record Particle::recordInit(const double left, const double right)
{
    TIME_GET_SEED(seedNew);
    seed = seedNew;
    std::uniform_real_distribution<double> randomX(left, right);
    std::uniform_real_distribution<double> randomV(Swarm::vMin, Swarm::vMax);

    const Individual individualInit{};
    Society societyInit(Swarm::size, individualInit);
    std::generate(
        societyInit.begin(), societyInit.end(),
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

// Genetic method
std::optional<std::tuple<ValueY, ValueX>> Genetic::operator()(
    const double left, const double right, const double eps)
{
    TIMER_BEGIN;
    updateSpecies(left, right, eps);
    if (chrNum < OPTIMUM_GENETIC_MIN_CHROMOSOME_NUMBER)
    {
        FORMAT_PRINT("*Genetic   method: The precise %.5f isn't enough.\n", eps);
        return std::nullopt;
    }

    Population pop = populationInit();
    for (uint32_t i = 0; i < Species::iterNum; ++i)
    {
        selectIndividual(pop);
        crossIndividual(pop);
        mutateIndividual(pop);
    }
    const double x = geneDecoding(getBestIndividual(pop));

    TIMER_END;
    FORMAT_PRINT(OPTIMUM_RESULT(max), "Genetic", func(x), x, TIMER_INTERVAL);
    return std::make_optional(std::make_tuple(func(x), x));
}

void Genetic::updateSpecies(const double left, const double right, const double eps)
{
    range.lower = left;
    range.upper = right;
    range.eps = eps;
    TIME_GET_SEED(seedNew);
    seed = seedNew;

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
        chr.begin(), chr.end(),
        [&]
        {
            return randomX(seed);
        });
}

double Genetic::geneDecoding(const Chromosome& chr) const
{
    const double max = std::pow(2, chrNum) - 1.0;
    double temp = 0.0;
    uint32_t i = 0;
    std::for_each(
        chr.cbegin(), chr.cend(),
        [&temp, &i](const auto& bit)
        {
            temp += bit * std::pow(2, i);
            ++i;
        });
    return range.lower + (range.upper - range.lower) * temp / max;
}

Population Genetic::populationInit()
{
    const Chromosome chrInit(chrNum, 0);
    Population pop(Species::size, chrInit);
    std::for_each(
        pop.begin(), pop.end(),
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

    uint32_t crossBegin = getRandomNumber(chrNum - 1);
    uint32_t crossEnd = getRandomNumber(chrNum - 1);
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
    for (auto iterChr = crossContainer.begin();
         (crossContainer.end() != iterChr) && (std::next(iterChr, 1) != crossContainer.end());
         std::advance(iterChr, 2))
    {
        Chromosome parent1 = iterChr->get(), parent2 = std::next(iterChr, 1)->get();
        if (Species::crossPr > random())
        {
            geneCrossover(parent1, parent2);
        }
        popCross.emplace_back(std::move(parent1));
        popCross.emplace_back(std::move(parent2));

        if ((pop.size() % 2) && (std::next(iterChr, 2) == crossContainer.end() - 1))
        {
            Chromosome single = std::next(iterChr, 2)->get();
            popCross.emplace_back(std::move(single));
        }
    }

    std::copy(popCross.cbegin(), popCross.cend(), pop.begin());
}

void Genetic::geneMutation(Chromosome& chr)
{
    uint32_t flip = getRandomNumber(chrNum - 1) + 1;
    std::vector<std::reference_wrapper<uint32_t>> mutateChr(chr.begin(), chr.end());
    std::shuffle(mutateChr.begin(), mutateChr.end(), seed);
    for (auto iterGene = mutateChr.begin(); (mutateChr.end() != iterGene) && (0 != flip);
         ++iterGene, --flip)
    {
        iterGene->get() = !iterGene->get();
    }
}

void Genetic::mutateIndividual(Population& pop)
{
    std::for_each(
        pop.begin(), pop.end(),
        [this](auto& ind)
        {
            if (Species::mutatePr > random())
            {
                geneMutation(ind);
            }
        });
}

double Genetic::calculateFitness(const Chromosome& chr)
{
    return func(geneDecoding(chr));
}

std::optional<std::pair<double, double>> Genetic::fitnessLinearTransformation(const Population& pop)
{
    std::vector<double> reFitness;
    reFitness.reserve(pop.size());
    std::transform(
        pop.cbegin(), pop.cend(), std::back_inserter(reFitness),
        [this](const auto& ind)
        {
            return calculateFitness(ind);
        });

    const double reFitnessMin = *(std::min_element(std::cbegin(reFitness), std::cend(reFitness)));
    const double reFitnessAvg =
        std::accumulate(std::cbegin(reFitness), std::cend(reFitness), 0.0) / reFitness.size();
    if (std::fabs(reFitnessMin - reFitnessAvg) > (range.eps * range.eps))
    {
        const double alpha = reFitnessAvg / (reFitnessAvg - reFitnessMin);
        const double beta = -1.0 * (reFitnessMin * reFitnessAvg) / (reFitnessAvg - reFitnessMin);
        assert(!std::isnan(alpha) && !std::isinf(alpha) && !std::isnan(beta) && !std::isinf(beta));
        return std::make_optional(std::pair<double, double>(alpha, beta));
    }
    else
    {
        return std::nullopt;
    }
}

auto Genetic::rouletteWheelSelection(const Population& pop, const std::vector<double>& fitnessCum)
{
    const double rand = random();
    const auto iterCum = std::find_if(
        fitnessCum.cbegin(), fitnessCum.cend(),
        [&rand](const auto& cumulation)
        {
            return cumulation > rand;
        });
    assert(iterCum != fitnessCum.cend());

    return std::next(pop.cbegin(), std::distance(fitnessCum.cbegin(), iterCum));
}

void Genetic::stochasticTournamentSelection(Population& pop, const std::vector<double>& fitnessCum)
{
    Population popNew;
    popNew.reserve(pop.size());
    while (popNew.size() < pop.size())
    {
        auto competitor1 = rouletteWheelSelection(pop, fitnessCum);
        auto competitor2 = rouletteWheelSelection(pop, fitnessCum);
        (calculateFitness(*competitor1) >= calculateFitness(*competitor2))
            ? popNew.emplace_back(*competitor1)
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
        pop.cbegin(), pop.cend(), std::back_inserter(fitnessVal),
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
        fitnessVal.cbegin(), fitnessVal.cend(), std::back_inserter(fitnessAvg),
        [&sum](const auto& fitVal)
        {
            return fitVal / sum;
        });

    double previous = 0.0;
    std::for_each(
        fitnessAvg.begin(), fitnessAvg.end(),
        [&previous](auto& fitCum)
        {
            fitCum += previous;
            previous = fitCum;
        });
    stochasticTournamentSelection(pop, fitnessAvg);
}

Chromosome Genetic::getBestIndividual(const Population& pop)
{
    std::vector<double> fitnessVal;
    fitnessVal.reserve(pop.size());
    std::transform(
        pop.cbegin(), pop.cend(), std::back_inserter(fitnessVal),
        [this](const auto& ind)
        {
            return calculateFitness(ind);
        });

    const auto iterFitValBest = std::max_element(std::cbegin(fitnessVal), std::cend(fitnessVal));
    const auto iterIndBest =
        std::next(pop.cbegin(), std::distance(fitnessVal.cbegin(), iterFitValBest));

    return *iterIndBest;
}
