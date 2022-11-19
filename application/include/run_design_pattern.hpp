#pragma once

#include <bitset>
#include <sstream>
#include <vector>

namespace run_dp
{
class DesignPatternTask
{
public:
    template <class T>
    struct Bottom;

    enum Type
    {
        behavioral,
        creational,
        structural
    };

    enum BehavioralInstance
    {
        chainOfResponsibility,
        command,
        interpreter,
        iterator,
        mediator,
        memento,
        observer,
        state,
        strategy,
        templateMethod,
        visitor
    };
    template <>
    struct Bottom<BehavioralInstance>
    {
        static constexpr int value = 11;
    };

    enum CreationalInstance
    {
        abstractFactory,
        builder,
        factoryMethod,
        prototype,
        singleton
    };
    template <>
    struct Bottom<CreationalInstance>
    {
        static constexpr int value = 5;
    };

    enum StructuralInstance
    {
        adapter,
        bridge,
        composite,
        decorator,
        facade,
        flyweight,
        proxy
    };
    template <>
    struct Bottom<StructuralInstance>
    {
        static constexpr int value = 7;
    };

    std::bitset<Bottom<BehavioralInstance>::value> behavioralBit;
    std::bitset<Bottom<CreationalInstance>::value> creationalBit;
    std::bitset<Bottom<StructuralInstance>::value> structuralBit;

    [[nodiscard]] bool empty() const { return (behavioralBit.none() && creationalBit.none() && structuralBit.none()); }
    void reset()
    {
        behavioralBit.reset();
        creationalBit.reset();
        structuralBit.reset();
    }

protected:
    friend std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        switch (type)
        {
            case Type::behavioral:
                os << "BEHAVIORAL";
                break;
            case Type::creational:
                os << "CREATIONAL";
                break;
            case Type::structural:
                os << "STRUCTURAL";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
        }
        return os;
    }
};

extern DesignPatternTask& getTask();

template <typename T>
auto getBit()
{
    if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralInstance>)
    {
        return getTask().behavioralBit;
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalInstance>)
    {
        return getTask().creationalBit;
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralInstance>)
    {
        return getTask().structuralBit;
    }
}

template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralInstance>)
    {
        getTask().behavioralBit.set(DesignPatternTask::BehavioralInstance(index));
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalInstance>)
    {
        getTask().creationalBit.set(DesignPatternTask::CreationalInstance(index));
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralInstance>)
    {
        getTask().structuralBit.set(DesignPatternTask::StructuralInstance(index));
    }
}

extern void runBehavioral(const std::vector<std::string>& targets);
extern void updateBehavioralTask(const std::string& target);
extern void runCreational(const std::vector<std::string>& targets);
extern void updateCreationalTask(const std::string& target);
extern void runStructural(const std::vector<std::string>& targets);
extern void updateStructuralTask(const std::string& target);
} // namespace run_dp
