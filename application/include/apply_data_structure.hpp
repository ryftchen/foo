//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once
#pragma once

#include <bitset>
#include <sstream>
#include <vector>

//! @brief Data-structure-applying-related functions in the application module.
namespace application::app_ds
{
//! @brief Manage data structure tasks.
class DataStructureTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T - type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific data structure tasks.
    enum Type
    {
        linear,
        tree
    };

    //! @brief Enumerate specific linear instances.
    enum LinearInstance
    {
        linkedList,
        stack,
        queue
    };
    //! @brief Store the maximum value of the LinearInstance enum.
    template <>
    struct Bottom<LinearInstance>
    {
        static constexpr int value = 3;
    };

    //! @brief Enumerate specific tree instances.
    enum TreeInstance
    {
        binarySearch,
        adelsonVelskyLandis,
        splay
    };
    //! @brief Store the maximum value of the TreeInstance enum.
    template <>
    struct Bottom<TreeInstance>
    {
        static constexpr int value = 3;
    };

    //! @brief Bit flags for managing linear instances.
    std::bitset<Bottom<LinearInstance>::value> linearBit;
    //! @brief Bit flags for managing tree instances.
    std::bitset<Bottom<TreeInstance>::value> treeBit;

    //! @brief Check whether any data structure tasks do not exist.
    //! @return any data structure tasks do not exist or exist
    [[nodiscard]] inline bool empty() const { return (linearBit.none() && treeBit.none()); }
    //! @brief Reset bit flags that manage data structure tasks.
    inline void reset()
    {
        linearBit.reset();
        treeBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Type enum.
    //! @param os - output stream object
    //! @param type - the specific value of Type enum
    //! @return reference of output stream object
    friend std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        switch (type)
        {
            case Type::linear:
                os << "LINEAR";
                break;
            case Type::tree:
                os << "TREE";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
        }
        return os;
    }
};

extern DataStructureTask& getTask();

//! @brief Get the bit flags of the instance in data structure tasks.
//! @tparam T - type of the instance
//! @return bit flags of the instance
template <typename T>
auto getBit()
{
    if constexpr (std::is_same_v<T, DataStructureTask::LinearInstance>)
    {
        return getTask().linearBit;
    }
    else if constexpr (std::is_same_v<T, DataStructureTask::TreeInstance>)
    {
        return getTask().treeBit;
    }
}

//! @brief Set the bit flags of the instance in data structure tasks.
//! @tparam T - type of the instance
//! @param index - instance index
template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, DataStructureTask::LinearInstance>)
    {
        getTask().linearBit.set(DataStructureTask::LinearInstance(index));
    }
    else if constexpr (std::is_same_v<T, DataStructureTask::TreeInstance>)
    {
        getTask().treeBit.set(DataStructureTask::TreeInstance(index));
    }
}

extern void runLinear(const std::vector<std::string>& targets);
extern void updateLinearTask(const std::string& target);
extern void runTree(const std::vector<std::string>& targets);
extern void updateTreeTask(const std::string& target);
} // namespace application::app_ds
