//! @file apply_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (apply_design_pattern) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <bitset>
#include <sstream>
#include <vector>

//! @brief Design-pattern-applying-related functions in the application module.
namespace application::app_dp
{
//! @brief Manage design pattern tasks.
class DesignPatternTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific design pattern tasks.
    enum Type
    {
        behavioral,
        creational,
        structural
    };

    //! @brief Enumerate specific behavioral instances.
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
    //! @brief Store the maximum value of the BehavioralInstance enum.
    //! @tparam N/A
    template <>
    struct Bottom<BehavioralInstance>
    {
        static constexpr int value = 11;
    };

    //! @brief Enumerate specific creational instances.
    enum CreationalInstance
    {
        abstractFactory,
        builder,
        factoryMethod,
        prototype,
        singleton
    };
    //! @brief Store the maximum value of the CreationalInstance enum.
    //! @tparam N/A
    template <>
    struct Bottom<CreationalInstance>
    {
        static constexpr int value = 5;
    };

    //! @brief Enumerate specific structural instances.
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
    //! @brief Store the maximum value of the StructuralInstance enum.
    //! @tparam N/A
    template <>
    struct Bottom<StructuralInstance>
    {
        static constexpr int value = 7;
    };

    //! @brief Bit flags for managing behavioral instances.
    std::bitset<Bottom<BehavioralInstance>::value> behavioralBit;
    //! @brief Bit flags for managing creational instances.
    std::bitset<Bottom<CreationalInstance>::value> creationalBit;
    //! @brief Bit flags for managing structural instances.
    std::bitset<Bottom<StructuralInstance>::value> structuralBit;

    //! @brief Check whether any design pattern tasks do not exist.
    //! @return any design pattern tasks do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return (behavioralBit.none() && creationalBit.none() && structuralBit.none());
    }
    //! @brief Reset bit flags that manage design pattern tasks.
    inline void reset()
    {
        behavioralBit.reset();
        creationalBit.reset();
        structuralBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Type enum.
    //! @param os output stream object
    //! @param type the specific value of Type enum
    //! @return reference of output stream object
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

//! @brief Get the bit flags of the instance in design pattern tasks.
//! @tparam T type of the instance
//! @return bit flags of the instance
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

//! @brief Set the bit flags of the instance in design pattern tasks.
//! @tparam T type of the instance
//! @param index instance index
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
} // namespace application::app_dp
