#pragma once

#include <bitset>
#include <sstream>
#include <vector>

namespace ds_tst
{
class DataStructureTask
{
public:
    template <class T>
    struct Bottom;

    enum Type
    {
        linear,
        tree
    };

    enum LinearInstance
    {
        linkedList,
        stack,
        queue
    };
    template <>
    struct Bottom<LinearInstance>
    {
        static constexpr int value = 3;
    };

    enum TreeInstance
    {
        binarySearch,
        adelsonVelskyLandis,
        splay
    };
    template <>
    struct Bottom<TreeInstance>
    {
        static constexpr int value = 3;
    };

    std::bitset<Bottom<LinearInstance>::value> linearBit;
    std::bitset<Bottom<TreeInstance>::value> treeBit;

    [[nodiscard]] bool empty() const { return (linearBit.none() && treeBit.none()); }
    void reset()
    {
        linearBit.reset();
        treeBit.reset();
    }

protected:
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
} // namespace ds_tst
