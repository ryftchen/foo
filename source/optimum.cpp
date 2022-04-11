#include "optimum.hpp"
#include <algorithm>
#include "log.hpp"

using Species::Chromosome;
using Species::Population;
using Swarm::Individual;
using Swarm::Record;
using Swarm::Society;

// Fibonacci method
std::optional<std::tuple<ValueY, ValueX>> Fibonacci::operator()(
    const double left, const double right, const double eps)
{
    TIME_BEGIN;
    double x = 0.0, y = 0.0;

    const auto firstResult = fibonacciSearch(left, right, eps);
    if (UNLIKELY(!firstResult.has_value()))
    {
        FORMAT_PRINT("*Fibonacci method: The precise %.5f isn't enough.\n", eps);
        return std::nullopt;
    }
    else
    {
        y = std::get<0>(firstResult.value());
        x = std::get<1>(firstResult.value());
        uint32_t n = 2, cnt = 0;
        do
        {
            std::vector<std::pair<ValueY, ValueX>> aggregation;
            aggregation.reserve(n);
            const double stepLength = (right - left) / n;
            for (uint32_t i = 0; i < n; ++i)
            {
                const auto result =
                    fibonacciSearch(left + i * stepLength, left + (i + 1) * stepLength, eps);
                if (result.has_value())
                {
                    aggregation.emplace_back(std::pair<ValueY, ValueX>(
                        std::get<0>(result.value()), std::get<1>(result.value())));
                }
            }
            std::sort(
                aggregation.begin(), aggregation.end(),
                [](const auto& max1, const auto& max2)
                {
                    return max1.first > max2.first;
                });

            if (aggregation.size())
            {
                const auto [yNew, xNew] = *aggregation.begin();
                if (std::fabs(yNew - y) <= eps)
                {
                    ++cnt;
                }
                if (yNew > y)
                {
                    x = xNew;
                    y = yNew;
                }
            }
            n *= 2;
        }
        while (cnt < OPTIMUM_FIBONACCI_UNCHANGED_TIMES);
    }

    TIME_END;
    FORMAT_PRINT(OPTIMUM_FIBONACCI, y, x, TIME_INTERVAL);
    return std::make_optional(std::make_tuple(y, x));
}
std::optional<std::pair<ValueY, ValueX>> Fibonacci::fibonacciSearch(
    const double left, const double right, const double eps)
{
    double leftVal = left, rightVal = right;
    std::vector<double> fibonacci(0);
    generateFibonacciNumber(fibonacci, (rightVal - leftVal) / eps);
    int n = fibonacci.size() - 1;
    if (n < OPTIMUM_FIBONACCI_MIN_COUNT)
    {
        return std::nullopt;
    }

    double x1 = OPTIMUM_FIBONACCI_X_1, x2 = OPTIMUM_FIBONACCI_X_2;
    while (n > OPTIMUM_FIBONACCI_MIN_COUNT)
    {
        if (fun(x1) < fun(x2))
        {
            leftVal = x1;
            x1 = x2;
            x2 = OPTIMUM_FIBONACCI_X_2;
        }
        else if (fun(x1) > fun(x2))
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
    if (fun(x1) < fun(x2))
    {
        leftVal = x1;
    }
    else if (fun(x1) > fun(x2))
    {
        rightVal = x2;
    }
    else
    {
        leftVal = x1;
        rightVal = x2;
    }
    const double x = (leftVal + rightVal) / 2.0;

    return std::make_optional(std::make_pair(fun(x), x));
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
    TIME_BEGIN;
    TIME_GET_SEED(seed);
    double x = 0.0, max = 0.0;
    std::uniform_real_distribution<double> randomX(left, right);
    std::vector<double> climbing;
    climbing.reserve(Learning::loopTime);
    while (climbing.size() < Learning::loopTime)
    {
        x = randomX(seed);
        if (climbing.end() == std::find(climbing.begin(), climbing.end(), x))
        {
            climbing.emplace_back(x);
        }
    }

    std::vector<std::pair<ValueY, ValueX>> aggregation;
    aggregation.reserve(climbing.size());
    for (const auto& climber : climbing)
    {
        x = climber;
        uint32_t i = 0;
        double learningRate = Learning::initialLearningRate;
        double gradient = calculateFirstDerivative(x, eps);
        double dx = learningRate * gradient;
        while ((std::fabs(dx) > eps) && ((x + dx) >= left) && ((x + dx) <= right))
        {
            x += dx;
            ++i;
            learningRate = Learning::initialLearningRate * 1.0 / (1.0 + Learning::decay * i);
            gradient = calculateFirstDerivative(x, eps);
            dx = learningRate * gradient;
        }
        aggregation.emplace_back(std::pair<ValueY, ValueX>(fun(x), x));
    }
    std::sort(
        aggregation.begin(), aggregation.end(),
        [](const auto& max1, const auto& max2)
        {
            return max1.first > max2.first;
        });
    max = std::get<0>(*aggregation.begin());
    x = std::get<1>(*aggregation.begin());

    TIME_END;
    FORMAT_PRINT(OPTIMUM_GRADIENT, max, x, TIME_INTERVAL);
    return std::make_optional(std::make_tuple(max, x));
}
double Gradient::calculateFirstDerivative(const double x, const double eps) const
{
    const double differential = eps / 2.0;
    return (fun(x + differential) - fun(x - differential)) / eps;
}

// Simulated annealing method
std::optional<std::tuple<ValueY, ValueX>> Annealing::operator()(
    const double left, const double right, const double eps)
{
    TIME_BEGIN;
    double temperature = Cooling::initialT;
    TIME_GET_SEED(seed);
    std::uniform_real_distribution<double> randomX(left, right);
    std::uniform_real_distribution<double> random(
        -OPTIMUM_ANNEALING_PERTURBATION, OPTIMUM_ANNEALING_PERTURBATION);
    double x = randomX(seed);
    double y = fun(x);
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
            yNew = fun(xNew);

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
        if (true == found)
        {
            x = xBest;
            y = yBest;
        }
        temperature *= Cooling::coolingRate;
    }

    TIME_END;
    FORMAT_PRINT(OPTIMUM_ANNEALING, y, x, TIME_INTERVAL);
    return std::make_optional(std::make_tuple(y, x));
}

// Particle swarm method
std::optional<std::tuple<ValueY, ValueX>> Particle::operator()(
    const double left, const double right, const double eps)
{
    TIME_BEGIN;
    Record rec = recordInit(left, right);
    const Society::iterator best = std::max_element(
        std::begin(rec.society), std::end(rec.society),
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
                : (ind.velocity < Swarm::vMin ? ind.velocity = Swarm::vMin : ind.velocity);

            ind.x += ind.velocity;
            (ind.x > right) ? ind.x = right : (ind.x < left ? ind.x = left : ind.x);

            ind.xFitness = fun(ind.x);
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
    xFitnessBest = std::get<0>(*(rec.history.begin()));
    xBest = std::get<1>(*(rec.history.begin()));

    TIME_END;
    FORMAT_PRINT(OPTIMUM_PARTICLE, xFitnessBest, xBest, TIME_INTERVAL);
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
            const Individual individual(x, randomV(seed), x, fun(x), fun(x));
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
    TIME_BEGIN;
    setSpecies(left, right, eps);
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

    TIME_END;
    FORMAT_PRINT(OPTIMUM_GENETIC, fun(x), x, TIME_INTERVAL);
    return std::make_optional(std::make_tuple(fun(x), x));
}
void Genetic::setSpecies(const double left, const double right, const double eps)
{
    range.left = left;
    range.right = right;
    range.eps = eps;
    TIME_GET_SEED(seedNew);
    seed = seedNew;

    uint32_t num = 0;
    const double max = (range.right - range.left) * (1.0 / range.eps);
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
        chr.begin(), chr.end(),
        [&temp, &i](const auto& bit)
        {
            temp += bit * std::pow(2, i);
            ++i;
        });
    return range.left + (range.right - range.left) * temp / max;
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
    std::copy(chr1.begin(), chr1.end(), std::back_inserter(chrTemp));

    uint32_t crossBegin = getRandomNumber(chrNum - 1);
    uint32_t crossEnd = getRandomNumber(chrNum - 1);
    if (crossBegin > crossEnd)
    {
        std::swap(crossBegin, crossEnd);
    }
    std::copy_n(chr2.begin() + crossBegin, crossEnd - crossBegin, chr1.begin() + crossBegin);
    std::copy_n(chrTemp.begin() + crossBegin, crossEnd - crossBegin, chr2.begin() + crossBegin);
}
void Genetic::crossIndividual(Population& pop)
{
    Population popCross;
    popCross.reserve(pop.size());
    Population popTemp(pop.size());
    std::copy(pop.begin(), pop.end(), popTemp.begin());
    while (popTemp.size() > 1)
    {
        const auto iterParent1 = popTemp.begin() + getRandomNumber(popTemp.size() - 1);
        Chromosome parent1 = *iterParent1;
        popTemp.erase(iterParent1);
        const auto iterParent2 = popTemp.begin() + getRandomNumber(popTemp.size() - 1);
        Chromosome parent2 = *iterParent2;
        popTemp.erase(iterParent2);
        if (Species::crossPr > random())
        {
            geneCrossover(parent1, parent2);
        }
        popCross.emplace_back(std::move(parent1));
        popCross.emplace_back(std::move(parent2));
    }
    if (0 != pop.size() % 2)
    {
        const auto iterRemainder = popTemp.begin();
        const Chromosome remainder = *iterRemainder;
        popTemp.erase(iterRemainder);
        popCross.emplace_back(remainder);
    }
    std::copy(popCross.begin(), popCross.end(), pop.begin());
}
void Genetic::geneMutation(Chromosome& chr)
{
    uint32_t flip = 0;
    do
    {
        flip = getRandomNumber(chrNum);
    }
    while (0 == flip);

    std::vector<uint32_t> index(chrNum);
    uint32_t id = 0;
    std::generate(
        index.begin(), index.end(),
        [&id]
        {
            return id++;
        });
    while (flip)
    {
        const auto iterId = index.begin() + getRandomNumber(index.size() - 1);
        auto iterBit = chr.begin() + *iterId;
        *iterBit = !*iterBit;
        index.erase(iterId);
        --flip;
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
    return fun(geneDecoding(chr));
}
std::optional<std::pair<double, double>> Genetic::fitnessLinearTransformation(const Population& pop)
{
    std::vector<double> reFitness;
    reFitness.reserve(pop.size());
    std::transform(
        pop.begin(), pop.end(), std::back_inserter(reFitness),
        [this](const auto& ind)
        {
            return calculateFitness(ind);
        });

    double reFitnessMin = *(std::min_element(std::begin(reFitness), std::end(reFitness)));
    double reFitnessAvg =
        std::accumulate(std::begin(reFitness), std::end(reFitness), 0.0) / reFitness.size();
    if (LIKELY(std::fabs(reFitnessMin - reFitnessAvg) > (range.eps * range.eps)))
    {
        const double alpha = reFitnessAvg / (reFitnessAvg - reFitnessMin);
        const double beta = -1.0 * (reFitnessMin * reFitnessAvg) / (reFitnessAvg - reFitnessMin);
        assert(!isnan(alpha) && !isinf(alpha) && !isnan(beta) && !isinf(beta));
        return std::make_optional(std::pair<double, double>(alpha, beta));
    }
    else
    {
        return std::nullopt;
    }
}
void Genetic::stochasticTournamentSelection(Population& pop, const std::vector<double>& fitnessCum)
{
    Population popNew;
    popNew.reserve(pop.size());
    while (popNew.size() < pop.size())
    {
        auto competitor1 = pop.end(), competitor2 = pop.end();
        do
        {
            const double rand = random();
            const auto iterCum = std::find_if(
                fitnessCum.begin(), fitnessCum.end(),
                [&rand](const auto& cumulation)
                {
                    return cumulation > rand;
                });
            assert(iterCum != fitnessCum.end());

            auto iterInd = pop.begin() + (iterCum - fitnessCum.begin());
            if (pop.end() == competitor1)
            {
                competitor1 = iterInd;
            }
            else if (pop.end() == competitor2)
            {
                competitor2 = iterInd;
            }
            if ((pop.end() != competitor1) && (pop.end() != competitor2))
            {
                if (calculateFitness(*competitor1) >= calculateFitness(*competitor2))
                {
                    popNew.emplace_back(*competitor1);
                }
                else
                {
                    popNew.emplace_back(*competitor2);
                }
            }
        }
        while ((pop.end() == competitor1) || (pop.end() == competitor2));
    }
    std::copy(popNew.begin(), popNew.end(), pop.begin());
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
        pop.begin(), pop.end(), std::back_inserter(fitnessVal),
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
        fitnessVal.begin(), fitnessVal.end(), std::back_inserter(fitnessAvg),
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
        pop.begin(), pop.end(), std::back_inserter(fitnessVal),
        [this](const auto& ind)
        {
            return calculateFitness(ind);
        });

    auto indBest = *(pop.begin());
    auto iterInd = pop.begin();
    auto fitValBest = *(fitnessVal.begin());
    for (auto fitVal : fitnessVal)
    {
        if (fitVal > fitValBest)
        {
            fitValBest = fitVal;
            indBest = *iterInd;
        }
        ++iterInd;
    }
    return indBest;
}
