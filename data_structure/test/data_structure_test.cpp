#include "data_structure_test.hpp"
#include "linear.hpp"
#include "tree.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

#define DATA_STRUCTURE_PRINT_TASK_BEGIN_TITLE(taskType)                                                               \
    std::cout << "\r\n"                                                                                               \
              << "DATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')                \
              << std::setw(50) << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << std::endl;                                                                                           \
    {
#define DATA_STRUCTURE_PRINT_TASK_END_TITLE(taskType)                                                               \
    }                                                                                                               \
    std::cout << "\r\n"                                                                                             \
              << "DATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')              \
              << std::setw(50) << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << "\r\n"                                                                                             \
              << std::endl;

namespace ds_tst
{
using util_hash::operator""_bkdrHash;
using Type = DataStructureTask::Type;
template <class T>
using Bottom = DataStructureTask::Bottom<T>;
using LinearInstance = DataStructureTask::LinearInstance;
using TreeInstance = DataStructureTask::TreeInstance;

DataStructureTask& getTask()
{
    static DataStructureTask task;
    return task;
}

void runLinear(const std::vector<std::string>& targets)
{
    if (getBit<LinearInstance>().none())
    {
        return;
    }

    DATA_STRUCTURE_PRINT_TASK_BEGIN_TITLE(Type::linear);

    using ds_linear::LinearStructure;
    const std::shared_ptr<LinearStructure> linear = std::make_shared<LinearStructure>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<LinearInstance>().count()), static_cast<uint32_t>(Bottom<LinearInstance>::value)));
    const auto linearFunctor = [&](const std::string& threadName, void (LinearStructure::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, linear);
    };

    for (int i = 0; i < Bottom<LinearInstance>::value; ++i)
    {
        if (!getBit<LinearInstance>().test(LinearInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "l_" + targetInstance;
        switch (util_hash::bkdrHash(targetInstance.data()))
        {
            case "lin"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::linkedListInstance);
                break;
            case "sta"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::stackInstance);
                break;
            case "que"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::queueInstance);
                break;
            default:
                LOG_DBG("execute to run unknown linear instance.");
                break;
        }
    }

    DATA_STRUCTURE_PRINT_TASK_END_TITLE(Type::linear);
}

void updateLinearTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
    {
        case "lin"_bkdrHash:
            setBit<LinearInstance>(LinearInstance::linkedList);
            break;
        case "sta"_bkdrHash:
            setBit<LinearInstance>(LinearInstance::stack);
            break;
        case "que"_bkdrHash:
            setBit<LinearInstance>(LinearInstance::queue);
            break;
        default:
            getBit<LinearInstance>().reset();
            throw std::runtime_error("Unexpected task of linear: " + target);
    }
}

void runTree(const std::vector<std::string>& targets)
{
    if (getBit<TreeInstance>().none())
    {
        return;
    }

    DATA_STRUCTURE_PRINT_TASK_BEGIN_TITLE(Type::tree);

    using ds_tree::TreeStructure;
    const std::shared_ptr<TreeStructure> tree = std::make_shared<TreeStructure>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<TreeInstance>().count()), static_cast<uint32_t>(Bottom<TreeInstance>::value)));
    const auto treeFunctor = [&](const std::string& threadName, void (TreeStructure::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, tree);
    };

    for (int i = 0; i < Bottom<TreeInstance>::value; ++i)
    {
        if (!getBit<TreeInstance>().test(TreeInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "t_" + targetInstance;
        switch (util_hash::bkdrHash(targetInstance.data()))
        {
            case "bin"_bkdrHash:
                treeFunctor(threadName, &TreeStructure::bsInstance);
                break;
            case "ade"_bkdrHash:
                treeFunctor(threadName, &TreeStructure::avlInstance);
                break;
            case "spl"_bkdrHash:
                treeFunctor(threadName, &TreeStructure::splayInstance);
                break;
            default:
                LOG_DBG("execute to run unknown tree instance.");
                break;
        }
    }

    DATA_STRUCTURE_PRINT_TASK_END_TITLE(Type::tree);
}

void updateTreeTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
    {
        case "bin"_bkdrHash:
            setBit<TreeInstance>(TreeInstance::binarySearch);
            break;
        case "ade"_bkdrHash:
            setBit<TreeInstance>(TreeInstance::adelsonVelskyLandis);
            break;
        case "spl"_bkdrHash:
            setBit<TreeInstance>(TreeInstance::splay);
            break;
        default:
            getBit<TreeInstance>().reset();
            throw std::runtime_error("Unexpected task of tree: " + target);
    }
}
} // namespace ds_tst
