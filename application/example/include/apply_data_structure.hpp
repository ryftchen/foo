//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <span>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "data_structure/include/cache.hpp"
#include "data_structure/include/filter.hpp"
#include "data_structure/include/linear.hpp"
#include "data_structure/include/tree.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Data-structure-applying-related functions in the application module.
namespace app_ds
{
//! @brief Apply cache.
namespace cache
{
//! @brief The version used to apply.
const char* const version = date_structure::cache::version();

//! @brief Separator for each KeyValue or KeyOptValue.
static constexpr std::string_view separator = ", ";
//! @brief Alias for the key.
using Key = char;
//! @brief Alias for the value.
using Value = std::string;
//! @brief Alias for the pair of key and value.
using KeyValue = std::pair<Key, Value>;
//! @brief The operator (<<) overloading of the KeyValue type.
//! @param os - output stream object
//! @param keyValue - specific KeyValue object
//! @return reference of the output stream object
static std::ostream& operator<<(std::ostream& os, const KeyValue& keyValue)
{
    os << '{' << keyValue.first << ": " << keyValue.second << '}';
    return os;
}
//! @brief Alias for the range of KeyValue.
using KeyValueRange = std::vector<KeyValue>;
//! @brief The operator (<<) overloading of the KeyValueRange type.
//! @param os - output stream object
//! @param keyValueRange - specific KeyValueRange object
//! @return reference of the output stream object
static std::ostream& operator<<(std::ostream& os, const KeyValueRange& keyValueRange)
{
    for (const auto& keyValue : keyValueRange)
    {
        os << keyValue << separator;
    }

    return os;
}
//! @brief Alias for the pair of key and optional value.
using KeyOptValue = std::pair<Key, std::optional<Value>>;
//! @brief The operator (<<) overloading of the KeyOptValue type.
//! @param os - output stream object
//! @param keyOptValue - specific KeyOptValue object
//! @return reference of the output stream object
static std::ostream& operator<<(std::ostream& os, const KeyOptValue& keyOptValue)
{
    os << '{' << keyOptValue.first << ": " << (keyOptValue.second.has_value() ? *keyOptValue.second : "---") << '}';
    return os;
}
//! @brief Alias for the range of KeyOptValue.
using KeyOptValueRange = std::vector<KeyOptValue>;
//! @brief The operator (<<) overloading of the KeyOptValueRange type.
//! @param os - output stream object
//! @param keyOptValueRange - specific KeyOptValueRange object
//! @return reference of the output stream object
static std::ostream& operator<<(std::ostream& os, const KeyOptValueRange& keyOptValueRange)
{
    for (const auto& keyOptValue : keyOptValueRange)
    {
        os << keyOptValue << separator;
    }

    return os;
}

//! @brief Showcase for cache instances.
class Showcase
{
public:
    //! @brief Destroy the Showcase object.
    virtual ~Showcase() = default;

    //! @brief First in first out.
    //! @return procedure output
    static std::ostringstream fifo()
    {
        const KeyValue keyValueA = {'A', "foo"}, keyValueB = {'B', "bar"}, keyValueC = {'C', "baz"},
                       keyValueD = {'D', "qux"};
        std::ostringstream process{};
        process << std::boolalpha;

        date_structure::cache::FIFO<Key, Value> fifoCache{3};
        process << "insert " << keyValueA << '\n';
        fifoCache.insert('A', "foo");
        process << "insert " << keyValueB << '\n';
        fifoCache.insert('B', "bar");
        process << "insert " << keyValueC << '\n';
        fifoCache.insert('C', "baz");
        process << "find A: " << fifoCache.find('A').has_value() << '\n';
        process << "find B: " << fifoCache.find('B').has_value() << '\n';
        process << "find B: " << fifoCache.find('B').has_value() << '\n';
        process << "find C: " << fifoCache.find('C').has_value() << '\n';
        process << "find A: " << fifoCache.find('A').has_value() << '\n';
        process << "erase D: " << fifoCache.erase('D') << '\n';
        process << "insert " << keyValueD << '\n';
        fifoCache.insert('D', "qux");
        process << "find range {A, B, C, D}: " << fifoCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(process.str().length() - separator.length());

        process << "\nerase range {B, C}: " << fifoCache.eraseRange(std::vector<Key>{'B', 'C'}) << '\n';
        auto resolvedRange =
            KeyOptValueRange{{'A', std::nullopt}, {'B', std::nullopt}, {'C', std::nullopt}, {'D', std::nullopt}};
        process << "resolve range " << resolvedRange;
        process.seekp(process.str().length() - separator.length());
        process << ": ";
        fifoCache.resolveRange(resolvedRange);
        process << resolvedRange;
        process.seekp(process.str().length() - separator.length());

        auto insertedRange = KeyValueRange{keyValueA, keyValueB, keyValueC, keyValueD};
        process << "\ninsert range " << insertedRange;
        process.seekp(process.str().length() - separator.length());
        fifoCache.insertRange(std::move(insertedRange));

        process << "\nwhether it is empty: " << fifoCache.empty() << '\n';
        process << "size: " << fifoCache.size() << '\n';
        process << "capacity: " << fifoCache.capacity() << '\n';
        process << "cache detail: " << fifoCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(process.str().length() - separator.length());

        return std::ostringstream{process.str().substr(0, process.tellp()) + '\n'};
    }

    //! @brief Least frequently used.
    //! @return procedure output
    static std::ostringstream lfu()
    {
        const KeyValue keyValueA = {'A', "foo"}, keyValueB = {'B', "bar"}, keyValueC = {'C', "baz"},
                       keyValueD = {'D', "qux"};
        std::ostringstream process{};
        process << std::boolalpha;

        date_structure::cache::LFU<Key, Value> lfuCache{3};
        process << "insert " << keyValueA << '\n';
        lfuCache.insert('A', "foo");
        process << "insert " << keyValueB << '\n';
        lfuCache.insert('B', "bar");
        process << "insert " << keyValueC << '\n';
        lfuCache.insert('C', "baz");
        process << "find A: " << lfuCache.find('A').has_value() << '\n';
        process << "find B: " << lfuCache.find('B').has_value() << '\n';
        process << "find B: " << lfuCache.find('B').has_value() << '\n';
        process << "find C: " << lfuCache.find('C').has_value() << '\n';
        process << "find A: " << lfuCache.find('A').has_value() << '\n';
        process << "erase D: " << lfuCache.erase('D') << '\n';
        process << "insert " << keyValueD << '\n';
        lfuCache.insert('D', "qux");
        process << "find range {A, B, C, D}: " << lfuCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(process.str().length() - separator.length());

        process << "\nerase range {B, C}: " << lfuCache.eraseRange(std::vector<Key>{'B', 'C'}) << '\n';
        auto resolvedRange =
            KeyOptValueRange{{'A', std::nullopt}, {'B', std::nullopt}, {'C', std::nullopt}, {'D', std::nullopt}};
        process << "resolve range " << resolvedRange;
        process.seekp(process.str().length() - separator.length());
        process << ": ";
        lfuCache.resolveRange(resolvedRange);
        process << resolvedRange;
        process.seekp(process.str().length() - separator.length());

        auto insertedRange = KeyValueRange{keyValueA, keyValueB, keyValueC, keyValueD};
        process << "\ninsert range " << insertedRange;
        process.seekp(process.str().length() - separator.length());
        lfuCache.insertRange(std::move(insertedRange));

        process << "\nwhether it is empty: " << lfuCache.empty() << '\n';
        process << "size: " << lfuCache.size() << '\n';
        process << "capacity: " << lfuCache.capacity() << '\n';
        process << "cache detail: " << lfuCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(process.str().length() - separator.length());

        return std::ostringstream{process.str().substr(0, process.tellp()) + '\n'};
    }

    //! @brief Least recently used.
    //! @return procedure output
    static std::ostringstream lru()
    {
        const KeyValue keyValueA = {'A', "foo"}, keyValueB = {'B', "bar"}, keyValueC = {'C', "baz"},
                       keyValueD = {'D', "qux"};
        std::ostringstream process{};
        process << std::boolalpha;

        date_structure::cache::LRU<Key, Value> lruCache{3};
        process << "insert " << keyValueA << '\n';
        lruCache.insert('A', "foo");
        process << "insert " << keyValueB << '\n';
        lruCache.insert('B', "bar");
        process << "insert " << keyValueC << '\n';
        lruCache.insert('C', "baz");
        process << "find A: " << lruCache.find('A').has_value() << '\n';
        process << "find B: " << lruCache.find('B').has_value() << '\n';
        process << "find B: " << lruCache.find('B').has_value() << '\n';
        process << "find C: " << lruCache.find('C').has_value() << '\n';
        process << "find A: " << lruCache.find('A').has_value() << '\n';
        process << "erase D: " << lruCache.erase('D') << '\n';
        process << "insert " << keyValueD << '\n';
        lruCache.insert('D', "qux");
        process << "find range {A, B, C, D}: " << lruCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(process.str().length() - separator.length());

        process << "\nerase range {B, C}: " << lruCache.eraseRange(std::vector<Key>{'B', 'C'}) << '\n';
        auto resolvedRange =
            KeyOptValueRange{{'A', std::nullopt}, {'B', std::nullopt}, {'C', std::nullopt}, {'D', std::nullopt}};
        process << "resolve range " << resolvedRange;
        process.seekp(process.str().length() - separator.length());
        process << ": ";
        lruCache.resolveRange(resolvedRange);
        process << resolvedRange;
        process.seekp(process.str().length() - separator.length());

        auto insertedRange = KeyValueRange{keyValueA, keyValueB, keyValueC, keyValueD};
        process << "\ninsert range " << insertedRange;
        process.seekp(process.str().length() - separator.length());
        lruCache.insertRange(std::move(insertedRange));

        process << "\nwhether it is empty: " << lruCache.empty() << '\n';
        process << "size: " << lruCache.size() << '\n';
        process << "capacity: " << lruCache.capacity() << '\n';
        process << "cache detail: " << lruCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(process.str().length() - separator.length());

        return std::ostringstream{process.str().substr(0, process.tellp()) + '\n'};
    }
};

//! @brief Structure of cache.
class CacheStructure
{
public:
    //! @brief Destroy the CacheStructure object.
    virtual ~CacheStructure() = default;

    //! @brief The first in first out instance.
    static void fifoInstance();
    //! @brief The least frequently used instance.
    static void lfuInstance();
    //! @brief The least recently used instance.
    static void lruInstance();
};
} // namespace cache
extern void applyingCache(const std::vector<std::string>& candidates);

//! @brief Apply filter.
namespace filter
{
//! @brief The version used to apply.
const char* const version = date_structure::filter::version();

//! @brief Showcase for filter instances.
class Showcase
{
public:
    //! @brief Destroy the Showcase object.
    virtual ~Showcase() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Bloom.
    //! @return procedure output
    static std::ostringstream bloom()
    {
        using namespace date_structure::filter::bloom;
        std::ostringstream process{};

        constexpr std::uint32_t totalSize = 1000;
        const std::string keyPart1 = "foo://bar/", keyPart2 = "/baz.qux";
        std::vector<std::string> urls1{}, urls2{};
        urls1.reserve(totalSize);
        urls1.reserve(totalSize);
        for (std::uint32_t i = 0; i < totalSize; ++i)
        {
            auto url1 = keyPart1;
            url1 += std::to_string(i);
            url1 += keyPart2;
            urls1.emplace_back(std::move(url1));

            auto url2 = keyPart1;
            url2 += std::to_string(i + totalSize);
            url2 += keyPart2;
            urls2.emplace_back(std::move(url2));
        }

        BloomFilter bf{};
        init(&bf, 0, 1e5, 1e-5);
        const std::uint32_t inserted = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&bf](const auto acc, const auto& url) { return acc + (insert(&bf, url.c_str(), url.length()) ? 1 : 0); });
        process << "insert {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << inserted << '\n';

        const std::uint32_t mayContained = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&bf](const auto acc, const auto& url)
            { return acc + (mayContain(&bf, url.c_str(), url.length()) ? 1 : 0); });
        process << "may contain {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << mayContained
                << '\n';
        const std::uint32_t mayNotContained = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&bf](const auto acc, const auto& url)
            { return acc + (!mayContain(&bf, url.c_str(), url.length()) ? 1 : 0); });
        process << "may not contain {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1)
                << "}: " << mayNotContained << '\n';
        clear(&bf);
        deinit(&bf);

        return process;
    }

    //! @brief Quotient.
    //! @return procedure output
    static std::ostringstream quotient()
    {
        using namespace date_structure::filter::quotient;
        std::ostringstream process{};
        process << std::boolalpha;

        constexpr std::uint32_t totalSize = 500;
        const std::string keyPart1 = "foo://bar/", keyPart2 = "/baz.qux";
        std::vector<std::string> urls1{}, urls2{};
        urls1.reserve(totalSize);
        urls1.reserve(totalSize);
        for (std::uint32_t i = 0; i < totalSize; ++i)
        {
            auto url1 = keyPart1;
            url1 += std::to_string(i);
            url1 += keyPart2;
            urls1.emplace_back(std::move(url1));

            auto url2 = keyPart1;
            url2 += std::to_string(i + totalSize);
            url2 += keyPart2;
            urls2.emplace_back(std::move(url2));
        }

        QuotientFilter qfA{}, qfB{};
        init(&qfA, 0, 16, 8);
        init(&qfB, 0, 16, 8);
        const std::uint32_t insertedA = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&qfA](const auto acc, const auto& url)
            { return acc + (insert(&qfA, url.c_str(), url.length()) ? 1 : 0); });
        process << "A insert {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << insertedA << '\n';
        const std::uint32_t insertedB = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&qfB](const auto acc, const auto& url)
            { return acc + (insert(&qfB, url.c_str(), url.length()) ? 1 : 0); });
        process << "B insert {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1) << "}: " << insertedB << '\n';

        QuotientFilter qfC{};
        process << "C merge A and B: " << merge(&qfA, &qfB, &qfC) << '\n';
        clear(&qfA);
        deinit(&qfA);
        clear(&qfB);
        deinit(&qfB);

        const std::uint32_t removedC = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&qfC](const auto acc, const auto& url)
            { return acc + (remove(&qfC, url.c_str(), url.length()) ? 1 : 0); });
        process << "C remove {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1) << "}: " << removedC << '\n';
        const std::uint32_t mayContainedC = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&qfC](const auto acc, const auto& url)
            { return acc + (mayContain(&qfC, url.c_str(), url.length()) ? 1 : 0); });
        process << "C may contain {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << mayContainedC
                << '\n';
        const std::uint32_t mayNotContainedC = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&qfC](const auto acc, const auto& url)
            { return acc + (!mayContain(&qfC, url.c_str(), url.length()) ? 1 : 0); });
        process << "C may not contain {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1)
                << "}: " << mayNotContainedC << '\n';
        clear(&qfC);
        deinit(&qfC);

        return process;
    }
    // NOLINTEND(google-build-using-namespace)
};

//! @brief Structure of filter.
class FilterStructure
{
public:
    //! @brief Destroy the FilterStructure object.
    virtual ~FilterStructure() = default;

    //! @brief The Bloom instance.
    static void bloomInstance();
    //! @brief The quotient instance.
    static void quotientInstance();
};
} // namespace filter
extern void applyingFilter(const std::vector<std::string>& candidates);

//! @brief Apply linear.
namespace linear
{
//! @brief The version used to apply.
const char* const version = date_structure::linear::version();

//! @brief Metadata, which is used in the instance.
struct Meta
{
    //! @brief Id of the metadata.
    int id{0};
    //! @brief Name of the metadata.
    char name[4]{'\0'};

    //! @brief The operator (<<) overloading of the Meta struct.
    //! @param os - output stream object
    //! @param meta - specific Meta object
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Meta& meta)
    {
        os << '{' << meta.id << ", " << meta.name << '}';
        return os;
    }
};

//! @brief Showcase for linear instances.
class Showcase
{
public:
    //! @brief Destroy the Showcase object.
    virtual ~Showcase() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Doubly linked list.
    //! @return procedure output
    static std::ostringstream dll()
    {
        using namespace date_structure::linear::doubly_linked_list;
        date_structure::linear::Output tracker{};
        auto& process = tracker.output();
        process << std::boolalpha;

        constexpr std::string_view separator = " <-> ";
        constexpr Meta meta[] = {{'a', "'a'"}, {'b', "'b'"}, {'c', "'c'"}, {'d', "'d'"}};
        const std::span<const Meta> nodes(meta);
        const Meta* val = nullptr;

        DLL dll = nullptr;
        create(&dll);
        insert(dll, 0, nodes.data());
        process << "insert (0) " << nodes[0] << '\n';
        insert(dll, 0, &nodes[1]);
        process << "insert (0) " << nodes[1] << '\n';
        insert(dll, 1, &nodes[2]);
        process << "insert (1) " << nodes[2] << '\n';
        remove(dll, 2);
        process << "remove (2)\n";

        insertFirst(dll, &nodes.front());
        process << "insert first " << nodes.front() << '\n';
        insertLast(dll, &nodes.back());
        process << "insert last " << nodes.back() << '\n';
        val = static_cast<Meta*>(getFirst(dll));
        process << "get first: " << *val << '\n';
        val = static_cast<Meta*>(getLast(dll));
        process << "get last: " << *val << '\n';
        removeFirst(dll);
        process << "remove first\n";
        removeLast(dll);
        process << "remove last\n";
        insert(dll, 1, nodes.data());
        process << "insert (1) " << nodes[0] << '\n';

        process << "whether it is empty: " << empty(dll) << '\n';
        process << "size: " << size(dll) << '\n';
        process << "linear details: HEAD -> ";
        for (int i = 0; i < size(dll); ++i)
        {
            val = static_cast<Meta*>(get(dll, i));
            process << *val << separator;
        }
        process.seekp(process.str().length() - separator.length());
        process << " -> NULL\n";
        destroy(&dll);

        return std::ostringstream{process.str()};
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        using namespace date_structure::linear::stack;
        date_structure::linear::Output tracker{};
        auto& process = tracker.output();
        process << std::boolalpha;

        constexpr std::string_view separator = ", ";
        constexpr Meta meta[] = {{'a', "'a'"}, {'b', "'b'"}, {'c', "'c'"}, {'d', "'d'"}};
        const std::span<const Meta> nodes(meta);
        const Meta* val = nullptr;

        Stack stk = nullptr;
        for (create(&stk); const auto& node : nodes)
        {
            push(stk, &node);
            process << "push " << node << '\n';
        }

        val = static_cast<Meta*>(pop(stk));
        process << "pop: " << *val << '\n';
        val = static_cast<Meta*>(top(stk));
        process << "top: " << *val << '\n';
        push(stk, &nodes.back());
        process << "push " << nodes.back() << '\n';

        process << "whether it is empty: " << empty(stk) << '\n';
        process << "size: " << size(stk) << '\n';
        process << "linear details: TOP [";
        while (!empty(stk))
        {
            val = static_cast<Meta*>(pop(stk));
            process << *val << separator;
        }
        process.seekp(process.str().length() - separator.length());
        process << "] BOTTOM\n";
        destroy(&stk);

        return std::ostringstream{process.str()};
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        using namespace date_structure::linear::queue;
        date_structure::linear::Output tracker{};
        auto& process = tracker.output();
        process << std::boolalpha;

        constexpr std::string_view separator = ", ";
        constexpr Meta meta[] = {{'a', "'a'"}, {'b', "'b'"}, {'c', "'c'"}, {'d', "'d'"}};
        const std::span<const Meta> nodes(meta);
        const Meta* val = nullptr;

        Queue que = nullptr;
        for (create(&que); const auto& node : nodes)
        {
            push(que, &node);
            process << "push " << node << '\n';
        }

        val = static_cast<Meta*>(pop(que));
        process << "pop: " << *val << '\n';
        val = static_cast<Meta*>(front(que));
        process << "front: " << *val << '\n';
        push(que, &nodes.front());
        process << "push " << nodes.front() << '\n';

        process << "whether it is empty: " << empty(que) << '\n';
        process << "size: " << size(que) << '\n';
        process << "linear details: FRONT [";
        while (!empty(que))
        {
            val = static_cast<Meta*>(pop(que));
            process << *val << separator;
        }
        process.seekp(process.str().length() - separator.length());
        process << "] REAR\n";
        destroy(&que);

        return std::ostringstream{process.str()};
    }
    // NOLINTEND(google-build-using-namespace)
};

//! @brief Structure of linear.
class LinearStructure
{
public:
    //! @brief Destroy the LinearStructure object.
    virtual ~LinearStructure() = default;

    //! @brief The doubly linked list instance.
    static void dllInstance();
    //! @brief The stack instance.
    static void stackInstance();
    //! @brief The queue instance.
    static void queueInstance();
};
} // namespace linear
extern void applyingLinear(const std::vector<std::string>& candidates);

//! @brief Apply tree.
namespace tree
{
//! @brief The version used to apply.
const char* const version = date_structure::tree::version();

//! @brief Showcase for tree instances.
class Showcase
{
public:
    //! @brief Destroy the Showcase object.
    virtual ~Showcase() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Binary search.
    //! @return procedure output
    static std::ostringstream bs()
    {
        using namespace date_structure::tree::bs;
        auto tracker = Output<std::int16_t>{};
        auto& process = tracker.output();
        constexpr std::array<std::int16_t, 6> nodes = {1, 5, 4, 3, 2, 6};

        BSTree tree{};
        tree.root = nullptr;
        tree.compare = [](const void* const a, const void* const b) -> int
        {
            const auto l = *static_cast<const std::int16_t*>(a), r = *static_cast<const std::int16_t*>(b);
            return (l > r) - (l < r);
        };
        process << "insertion ";
        for (const auto& node : nodes)
        {
            process << node << ", ";
            insertion(&tree, &node);
        }
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        tracker.preOrderTraversal(tree.root);
        process << "\nin-order traversal: ";
        tracker.inOrderTraversal(tree.root);
        process << "\npost-order traversal: ";
        tracker.postOrderTraversal(tree.root);

        process << "\nminimum: " << *static_cast<std::int16_t*>(getMinimum(&tree)->key);
        process << "\nmaximum: " << *static_cast<std::int16_t*>(getMaximum(&tree)->key);
        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);

        constexpr std::int16_t deleteNode = 3;
        process << "deletion " << deleteNode;
        deletion(&tree, &deleteNode);

        process << "\nin-order traversal: ";
        tracker.inOrderTraversal(tree.root);
        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);
        destruction(&tree);

        return std::ostringstream{process.str()};
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        using namespace date_structure::tree::avl;
        auto tracker = Output<std::int16_t>{};
        auto& process = tracker.output();
        constexpr std::array<std::int16_t, 16> nodes = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};

        AVLTree tree{};
        tree.root = nullptr;
        tree.compare = [](const void* const a, const void* const b) -> int
        {
            const auto l = *static_cast<const std::int16_t*>(a), r = *static_cast<const std::int16_t*>(b);
            return (l > r) - (l < r);
        };
        process << "height: " << getHeight(&tree);
        process << "\ninsertion ";
        for (const auto& node : nodes)
        {
            process << node << ", ";
            insertion(&tree, &node);
        }
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        tracker.preOrderTraversal(tree.root);
        process << "\nin-order traversal: ";
        tracker.inOrderTraversal(tree.root);
        process << "\npost-order traversal: ";
        tracker.postOrderTraversal(tree.root);

        process << "\nheight: " << getHeight(&tree);
        process << "\nminimum: " << *static_cast<std::int16_t*>(getMinimum(&tree)->key);
        process << "\nmaximum: " << *static_cast<std::int16_t*>(getMaximum(&tree)->key);
        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);

        constexpr std::int16_t deleteNode = 8;
        process << "deletion " << deleteNode;
        deletion(&tree, &deleteNode);

        process << "\nheight: " << getHeight(&tree);
        process << "\nin-order traversal: ";
        tracker.inOrderTraversal(tree.root);
        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);
        destruction(&tree);

        return std::ostringstream{process.str()};
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        using namespace date_structure::tree::splay;
        auto tracker = Output<std::int16_t>{};
        auto& process = tracker.output();
        constexpr std::array<std::int16_t, 7> nodes = {10, 50, 40, 70, 30, 20, 60};

        SplayTree tree{};
        tree.root = nullptr;
        tree.compare = [](const void* const a, const void* const b) -> int
        {
            const auto l = *static_cast<const std::int16_t*>(a), r = *static_cast<const std::int16_t*>(b);
            return (l > r) - (l < r);
        };
        process << "insertion ";
        for (const auto& node : nodes)
        {
            process << node << ", ";
            insertion(&tree, &node);
        }
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        tracker.preOrderTraversal(tree.root);
        process << "\nin-order traversal: ";
        tracker.inOrderTraversal(tree.root);
        process << "\npost-order traversal: ";
        tracker.postOrderTraversal(tree.root);

        process << "\nminimum: " << *static_cast<std::int16_t*>(getMinimum(&tree)->key);
        process << "\nmaximum: " << *static_cast<std::int16_t*>(getMaximum(&tree)->key);
        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);

        constexpr std::int16_t deleteNode = 70;
        process << "deletion " << deleteNode;
        deletion(&tree, &deleteNode);

        process << "\nin-order traversal: ";
        tracker.inOrderTraversal(tree.root);
        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);

        constexpr std::int16_t splayNode = 30;
        process << "splaying " << splayNode;
        splaying(&tree, &splayNode);

        process << "\ntree details:\n";
        tracker.traverse(tree.root, tree.root->key, 0);
        destruction(&tree);

        return std::ostringstream{process.str()};
    }
    // NOLINTEND(google-build-using-namespace)
};

//! @brief Structure of tree.
class TreeStructure
{
public:
    //! @brief Destroy the TreeStructure object.
    virtual ~TreeStructure() = default;

    //! @brief The binary search instance.
    static void bsInstance();
    //! @brief The Adelson-Velsky-Landis instance.
    static void avlInstance();
    //! @brief The splay instance.
    static void splayInstance();
};
} // namespace tree
extern void applyingTree(const std::vector<std::string>& candidates);
} // namespace app_ds
} // namespace application
