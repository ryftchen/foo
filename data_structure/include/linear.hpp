//! @file linear.hpp
//! @author ryftchen
//! @brief The declarations (linear) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <ostream>

//! @brief The data structure module.
namespace date_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Linear-related functions in the data structure module.
namespace linear
{
extern const char* version() noexcept;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 8)
    //! @brief The node of the linear structure.
    typedef struct TagNode
    {
        //! @brief Pointer to the previous node.
        struct TagNode* prev;
        //! @brief Pointer to the next node.
        struct TagNode* next;
        //! @brief Node element.
        void* elem;
    } Node, *Linear;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

//! @brief The doubly linked list structure.
namespace dll
{
//! @brief Alias for the linear structure. Used for the doubly linked list.
using DLL = Linear;

extern bool create(DLL* const dll);
extern bool destroy(DLL* const dll);
extern int size(const DLL head);
extern bool empty(const DLL head);
extern void* get(const DLL head, const int index);
extern void* getFirst(const DLL head);
extern void* getLast(const DLL head);
extern bool insert(const DLL head, const int index, const void* const value);
extern bool insertFirst(const DLL head, const void* const value);
extern bool insertLast(const DLL head, const void* const value);
extern bool remove(const DLL head, const int index);
extern bool removeFirst(const DLL head);
extern bool removeLast(const DLL head);

//! @brief Print the doubly linked list.
//! @tparam Elem - type of element
template <typename Elem>
class Printer
{
public:
    //! @brief Construct a new Printer object.
    //! @param dll - doubly linked list to print
    explicit Printer(const DLL* const dll) : dll{dll} {}
    //! @brief Destroy the Printer object.
    virtual ~Printer() = default;

private:
    //! @brief The doubly linked list to print.
    const DLL* const dll{nullptr};

protected:
    template <typename E>
    friend std::ostream& operator<<(std::ostream&, const Printer<E>&);
};

//! @brief The operator (<<) overloading of the Printer class.
//! @tparam E - type of element
//! @param os - output stream object
//! @param printer - specific Printer object
//! @return reference of the output stream object
template <typename E>
std::ostream& operator<<(std::ostream& os, const Printer<E>& printer)
{
    if (printer.dll && *(printer.dll))
    {
        const auto& dll = *(printer.dll);
        const int size = dll::size(dll);
        os << "HEAD -> ";
        for (int i = 0; i < size; ++i)
        {
            os << *static_cast<const E*>(get(dll, i));
            if (i < (size - 1))
            {
                os << " <-> ";
            }
        }
        os << " <- TAIL";
    }

    return os;
}
} // namespace dll

//! @brief The stack structure (FILO/LIFO).
namespace stack
{
//! @brief Alias for the linear structure. Used for the stack.
using Stack = Linear;

extern bool create(Stack* const stack);
extern bool destroy(Stack* const stack);
extern int size(const Stack head);
extern bool empty(const Stack head);
extern bool push(const Stack head, const void* const value);
extern void* top(const Stack head);
extern void* pop(const Stack head);

//! @brief Print the stack.
//! @tparam Elem - type of element
template <typename Elem>
class Printer
{
public:
    //! @brief Construct a new Printer object.
    //! @param stk - stack to print
    explicit Printer(const Stack* const stk) : stk{stk} {}
    //! @brief Destroy the Printer object.
    virtual ~Printer() = default;

private:
    //! @brief The stack to print.
    const Stack* const stk{nullptr};

protected:
    template <typename E>
    friend std::ostream& operator<<(std::ostream&, const Printer<E>&);
};

//! @brief The operator (<<) overloading of the Printer class.
//! @tparam E - type of element
//! @param os - output stream object
//! @param printer - specific Printer object
//! @return reference of the output stream object
template <typename E>
std::ostream& operator<<(std::ostream& os, const Printer<E>& printer)
{
    if (printer.stk && *(printer.stk))
    {
        const auto& stk = *(printer.stk);
        const int size = dll::size(stk);
        os << "TOP [ ";
        for (int i = 0; i < size; ++i)
        {
            os << *static_cast<const E*>(dll::get(stk, i));
            if (i < (size - 1))
            {
                os << " | ";
            }
        }
        os << " ] BOTTOM";
    }

    return os;
}
} // namespace stack

//! @brief The queue structure (FIFO/LILO).
namespace queue
{
//! @brief Alias for the linear structure. Used for the queue.
using Queue = Linear;

extern bool create(Queue* const queue);
extern bool destroy(Queue* const queue);
extern int size(const Queue head);
extern bool empty(const Queue head);
extern bool push(const Queue head, const void* const value);
extern void* front(const Queue head);
extern void* pop(const Queue head);

//! @brief Print the queue.
//! @tparam Elem - type of element
template <typename Elem>
class Printer
{
public:
    //! @brief Construct a new Printer object.
    //! @param que - queue to print
    explicit Printer(const Queue* const que) : que{que} {}
    //! @brief Destroy the Printer object.
    virtual ~Printer() = default;

private:
    //! @brief The queue to print.
    const Queue* const que{nullptr};

protected:
    template <typename E>
    friend std::ostream& operator<<(std::ostream&, const Printer<E>&);
};

//! @brief The operator (<<) overloading of the Printer class.
//! @tparam E - type of element
//! @param os - output stream object
//! @param printer - specific Printer object
//! @return reference of the output stream object
template <typename E>
std::ostream& operator<<(std::ostream& os, const Printer<E>& printer)
{
    if (printer.que && *(printer.que))
    {
        const auto& que = *(printer.que);
        const int size = dll::size(que);
        os << "FRONT [ ";
        for (int i = 0; i < size; ++i)
        {
            os << *static_cast<const E*>(dll::get(que, i));
            if (i < (size - 1))
            {
                os << " | ";
            }
        }
        os << " ] REAR";
    }

    return os;
}
} // namespace queue

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param head - head of linear structure for traversing
    explicit Traverse(const dll::DLL* const head) : head{head} {}
    //! @brief Destroy the Traverse object.
    virtual ~Traverse() = default;

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a order traversal starting from head.
    //! @param op - operation on each node
    void order(const Operation& op) const;
    //! @brief Perform a reverse traversal starting from tail.
    //! @param op - operation on each node
    void reverse(const Operation& op) const;

private:
    //! @brief The head of linear structure for traversing.
    const dll::DLL* const head{nullptr};
};
} // namespace linear
} // namespace date_structure
