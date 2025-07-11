//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "data_structure/include/cache.hpp"
#include "data_structure/include/filter.hpp"
#include "data_structure/include/graph.hpp"
#include "data_structure/include/heap.hpp"
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

        date_structure::cache::FIFO<Key, Value> fifoCache{3};
        process << std::boolalpha;
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
        process << "current status: " << fifoCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
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

        date_structure::cache::LFU<Key, Value> lfuCache{3};
        process << std::boolalpha;
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
        process << "current status: " << lfuCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
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

        date_structure::cache::LRU<Key, Value> lruCache{3};
        process << std::boolalpha;
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
        process << "current status: " << lruCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
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

    //! @brief Bloom.
    //! @return procedure output
    static std::ostringstream bloom()
    {
        constexpr std::uint32_t totalSize = 1000;
        const std::string keyPart1 = "foo://bar/", keyPart2 = "/baz.qux";
        std::ostringstream process{};
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

        date_structure::filter::Bloom bf(1e5, 1e-5, 0);
        process << std::boolalpha;
        const std::uint32_t inserted = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&bf](const auto acc, const auto& url) { return acc + (bf.insert(url.c_str(), url.length()) ? 1 : 0); });
        process << "insert {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << inserted << '\n';

        const std::uint32_t mayContained = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&bf](const auto acc, const auto& url)
            { return acc + (bf.mayContain(url.c_str(), url.length()) ? 1 : 0); });
        process << "may contain {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << mayContained
                << '\n';
        const std::uint32_t mayNotContained = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&bf](const auto acc, const auto& url)
            { return acc + (!bf.mayContain(url.c_str(), url.length()) ? 1 : 0); });
        process << "may not contain {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1)
                << "}: " << mayNotContained << '\n';
        bf.clear();

        return process;
    }

    //! @brief Quotient.
    //! @return procedure output
    static std::ostringstream quotient()
    {
        constexpr std::uint32_t totalSize = 500;
        const std::string keyPart1 = "foo://bar/", keyPart2 = "/baz.qux";
        std::ostringstream process{};
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

        date_structure::filter::Quotient qfA(16, 8, 0), qfB(16, 8, 0), qfC(16, 8, 0);
        process << std::boolalpha;
        const std::uint32_t insertedA = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&qfA](const auto acc, const auto& url) { return acc + (qfA.insert(url.c_str(), url.length()) ? 1 : 0); });
        process << "A insert {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << insertedA << '\n';
        const std::uint32_t insertedB = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&qfB](const auto acc, const auto& url) { return acc + (qfB.insert(url.c_str(), url.length()) ? 1 : 0); });
        process << "B insert {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1) << "}: " << insertedB << '\n';

        process << "C merge A and B: " << qfC.merge(qfA, qfB) << '\n';
        qfA.clear();
        qfB.clear();

        const std::uint32_t removedC = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&qfC](const auto acc, const auto& url) { return acc + (qfC.remove(url.c_str(), url.length()) ? 1 : 0); });
        process << "C remove {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1) << "}: " << removedC << '\n';
        const std::uint32_t mayContainedC = std::accumulate(
            urls1.cbegin(),
            urls1.cend(),
            0,
            [&qfC](const auto acc, const auto& url)
            { return acc + (qfC.mayContain(url.c_str(), url.length()) ? 1 : 0); });
        process << "C may contain {" << urls1.at(0) << " ... " << urls1.at(urls1.size() - 1) << "}: " << mayContainedC
                << '\n';
        const std::uint32_t mayNotContainedC = std::accumulate(
            urls2.cbegin(),
            urls2.cend(),
            0,
            [&qfC](const auto acc, const auto& url)
            { return acc + (!qfC.mayContain(url.c_str(), url.length()) ? 1 : 0); });
        process << "C may not contain {" << urls2.at(0) << " ... " << urls2.at(urls2.size() - 1)
                << "}: " << mayNotContainedC << '\n';
        qfC.clear();

        return process;
    }
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

//! @brief Apply graph.
namespace graph
{
//! @brief The version used to apply.
const char* const version = date_structure::graph::version();

//! @brief Alias for the data.
using Data = char;
//! @brief Compare function for the data.
//! @param a - first data
//! @param b - second data
//! @return a is less than b if returns -1, a is greater than b if returns 1, and a is equal to b if returns 0
static int compareData(const void* const a, const void* const b)
{
    const auto l = *static_cast<const Data*>(a), r = *static_cast<const Data*>(b);
    return (l > r) - (l < r);
}

//! @brief Showcase for graph instances.
class Showcase
{
public:
    //! @brief Destroy the Showcase object.
    virtual ~Showcase() = default;

    // NOLINTBEGIN(google-build-using-namespace, readability-magic-numbers)
    //! @brief Undirected.
    //! @return procedure output
    static std::ostringstream undirected()
    {
        using namespace date_structure::graph::undirected;
        constexpr std::array<Data, 7> vertices = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
        constexpr std::array<std::array<Data, 2>, 7> edges = {
            {{{'A', 'C'}}, {{'A', 'D'}}, {{'A', 'F'}}, {{'B', 'C'}}, {{'C', 'D'}}, {{'E', 'G'}}, {{'F', 'G'}}}};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const data)
        { process << *static_cast<const Data*>(data) << ' '; };

        AMLGraph* const graph = create(&compareData);
        const auto traverse = Traverse(graph);
        process << std::boolalpha;
        for (const auto& vertex : vertices)
        {
            process << "insert vertex " << vertex << ": " << insertVertex(graph, &vertex) << '\n';
        }
        for (const auto& edge : edges)
        {
            process << "insert edge " << edge[0] << '-' << edge[1] << ": " << insertEdge(graph, edge.data(), &edge[1])
                    << '\n';
        }

        process << "DFS traversal from " << vertices[0] << ": ";
        traverse.dfs(vertices.data(), opInTraversal);
        process << "\nBFS traversal from " << vertices[0] << ": ";
        traverse.bfs(vertices.data(), opInTraversal);

        process << "\ndelete edge " << edges[1][0] << '-' << edges[1][1] << ": "
                << deleteEdge(graph, edges[1].data(), &edges[1][1]) << '\n';
        process << "delete vertex " << vertices[0] << ": " << deleteVertex(graph, vertices.data()) << '\n';

        process << "DFS traversal from " << vertices[1] << ": ";
        traverse.dfs(&vertices[1], opInTraversal);
        process << "\nBFS traversal from " << vertices[1] << ": ";
        traverse.bfs(&vertices[1], opInTraversal);

        process << "\nDFS traversal from " << vertices[5] << ": ";
        traverse.dfs(&vertices[5], opInTraversal);
        process << "\nBFS traversal from " << vertices[5] << ": ";
        traverse.bfs(&vertices[5], opInTraversal);
        process << '\n';
        destroy(graph);

        return std::ostringstream{process.str()};
    }

    //! @brief Directed.
    //! @return procedure output
    static std::ostringstream directed()
    {
        using namespace date_structure::graph::directed;
        constexpr std::array<Data, 7> vertices = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
        constexpr std::array<std::array<Data, 2>, 9> edges = {
            {{{'A', 'B'}},
             {{'B', 'C'}},
             {{'B', 'E'}},
             {{'B', 'F'}},
             {{'C', 'E'}},
             {{'D', 'C'}},
             {{'E', 'B'}},
             {{'E', 'D'}},
             {{'F', 'G'}}}};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const data)
        { process << *static_cast<const Data*>(data) << ' '; };

        OLGraph* const graph = create(&compareData);
        const auto traverse = Traverse(graph);
        process << std::boolalpha;
        for (const auto& vertex : vertices)
        {
            process << "insert vertex " << vertex << ": " << insertVertex(graph, &vertex) << '\n';
        }
        for (const auto& edge : edges)
        {
            process << "insert arc " << edge[0] << '-' << edge[1] << ": " << insertArc(graph, edge.data(), &edge[1])
                    << '\n';
        }

        process << "DFS traversal from " << vertices[0] << ": ";
        traverse.dfs(vertices.data(), opInTraversal);
        process << "\nBFS traversal from " << vertices[0] << ": ";
        traverse.bfs(vertices.data(), opInTraversal);

        process << "\ndelete arc " << edges[6][0] << '-' << edges[6][1] << ": "
                << deleteArc(graph, edges[6].data(), &edges[6][1]) << '\n';
        process << "delete vertex " << vertices[1] << ": " << deleteVertex(graph, &vertices[1]) << '\n';

        process << "DFS traversal from " << vertices[0] << ": ";
        traverse.dfs(vertices.data(), opInTraversal);
        process << "\nBFS traversal from " << vertices[0] << ": ";
        traverse.bfs(vertices.data(), opInTraversal);

        process << "\nDFS traversal from " << vertices[2] << ": ";
        traverse.dfs(&vertices[2], opInTraversal);
        process << "\nBFS traversal from " << vertices[2] << ": ";
        traverse.bfs(&vertices[2], opInTraversal);

        process << "\nDFS traversal from " << vertices[5] << ": ";
        traverse.dfs(&vertices[5], opInTraversal);
        process << "\nBFS traversal from " << vertices[5] << ": ";
        traverse.bfs(&vertices[5], opInTraversal);
        process << '\n';
        destroy(graph);

        return std::ostringstream{process.str()};
    }
    // NOLINTEND(google-build-using-namespace, readability-magic-numbers)
};

//! @brief Structure of graph.
class GraphStructure
{
public:
    //! @brief Destroy the GraphStructure object.
    virtual ~GraphStructure() = default;

    //! @brief The undirected instance.
    static void undirectedInstance();
    //! @brief The directed instance.
    static void directedInstance();
};
} // namespace graph
extern void applyingGraph(const std::vector<std::string>& candidates);

//! @brief Apply heap.
namespace heap
{
//! @brief The version used to apply.
const char* const version = date_structure::heap::version();

//! @brief Alias for the key.
using Key = std::int16_t;
//! @brief Compare function for the key.
//! @param a - first key
//! @param b - second key
//! @return a is less than b if returns -1, a is greater than b if returns 1, and a is equal to b if returns 0
static int compareData(const void* const a, const void* const b)
{
    const auto l = *static_cast<const Key*>(a), r = *static_cast<const Key*>(b);
    return (l > r) - (l < r);
}

//! @brief Showcase for heap instances.
class Showcase
{
public:
    //! @brief Destroy the Showcase object.
    virtual ~Showcase() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Max.
    //! @return procedure output
    static std::ostringstream max()
    {
        using namespace date_structure::heap::max;
        using date_structure::heap::max::Traverse;
        constexpr std::array<Key, 9> keys = {10, 40, 30, 60, 90, 70, 20, 50, 80};
        constexpr int capacity = 30;
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        MaxHeap* const heap = creation(capacity, &compareData);
        const auto traverse = Traverse(heap);
        process << std::boolalpha;
        for (const auto& key : keys)
        {
            process << "insert " << key << ": " << insertion(heap, &key) << '\n';
        }
        process << "traversal: ";
        traverse.order(opInTraversal);

        constexpr Key insertedKey = 85;
        process << "\ninsert " << insertedKey << ": " << insertion(heap, &insertedKey) << '\n';
        process << "traversal: ";
        traverse.order(opInTraversal);

        constexpr Key removedKey = 90;
        process << "\nremove " << removedKey << ": " << deletion(heap, &removedKey) << '\n';
        process << "traversal: ";
        traverse.order(opInTraversal);
        process << '\n';
        destruction(heap);

        return std::ostringstream{process.str()};
    }

    //! @brief Min.
    //! @return procedure output
    static std::ostringstream min()
    {
        using namespace date_structure::heap::min;
        using date_structure::heap::min::Traverse;
        constexpr std::array<Key, 9> keys = {80, 40, 30, 60, 90, 70, 10, 50, 20};
        constexpr int capacity = 30;
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        MinHeap* const heap = creation(capacity, &compareData);
        const auto traverse = Traverse(heap);
        process << std::boolalpha;
        for (const auto& key : keys)
        {
            process << "insert " << key << ": " << insertion(heap, &key) << '\n';
        }
        process << "traversal: ";
        traverse.order(opInTraversal);

        constexpr Key insertedKey = 15;
        process << "\ninsert " << insertedKey << ": " << insertion(heap, &insertedKey) << '\n';
        process << "traversal: ";
        traverse.order(opInTraversal);

        constexpr Key removedKey = 10;
        process << "\nremove " << removedKey << ": " << deletion(heap, &removedKey) << '\n';
        process << "traversal: ";
        traverse.order(opInTraversal);
        process << '\n';
        destruction(heap);

        return std::ostringstream{process.str()};
    }
    // NOLINTEND(google-build-using-namespace)
};

//! @brief Structure of heap.
class HeapStructure
{
public:
    //! @brief Destroy the HeapStructure object.
    virtual ~HeapStructure() = default;

    //! @brief The max instance.
    static void maxInstance();
    //! @brief The min instance.
    static void minInstance();
};
} // namespace heap
extern void applyingHeap(const std::vector<std::string>& candidates);

//! @brief Apply linear.
namespace linear
{
//! @brief The version used to apply.
const char* const version = date_structure::linear::version();

//! @brief Alias for the element.
using Elem = std::int16_t;

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
        using namespace date_structure::linear::dll;
        using date_structure::linear::Traverse;
        using Printer = Printer<Elem>;
        constexpr std::array<Elem, 4> elems = {'a', 'b', 'c', 'd'};
        std::ostringstream process{};

        DLL dll{};
        create(&dll);
        process << std::boolalpha;
        for (const auto& elem : elems)
        {
            process << "insert (0) " << elem << ": " << insert(dll, 0, &elem) << '\n';
        }
        process << "traversal: ";
        Traverse(&dll).order([&process](const void* const elem)
                             { process << *static_cast<const Elem*>(elem) << " ... "; });

        process << "\nremove (1): " << remove(dll, 1) << '\n';
        process << "insert (2) " << elems[2] << ": " << insert(dll, 2, &elems[2]) << '\n';

        process << "insert first " << elems[0] << ": " << insertFirst(dll, elems.data()) << '\n';
        process << "insert last " << elems[3] << ": " << insertLast(dll, &elems[3]) << '\n';
        process << "get first: " << *static_cast<Elem*>(getFirst(dll)) << '\n';
        process << "get last: " << *static_cast<Elem*>(getLast(dll)) << '\n';
        process << "remove first: " << removeFirst(dll) << '\n';
        process << "remove last: " << removeLast(dll) << '\n';

        process << "whether it is empty: " << empty(dll) << '\n';
        process << "size: " << size(dll) << '\n';
        process << "all details: " << Printer(&dll) << '\n';
        destroy(&dll);

        return std::ostringstream{process.str()};
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        using namespace date_structure::linear::stack;
        using date_structure::linear::Traverse;
        using Printer = Printer<Elem>;
        constexpr std::array<Elem, 4> elems = {'a', 'b', 'c', 'd'};
        std::ostringstream process{};

        Stack stk{};
        create(&stk);
        process << std::boolalpha;
        for (const auto& elem : elems)
        {
            process << "push " << elem << ": " << push(stk, &elem) << '\n';
        }
        process << "traversal: ";
        Traverse(&stk).order([&process](const void* const elem)
                             { process << *static_cast<const Elem*>(elem) << " ... "; });

        process << "\npop: " << *static_cast<Elem*>(pop(stk)) << '\n';
        process << "top: " << *static_cast<Elem*>(top(stk)) << '\n';
        process << "push " << elems[3] << ": " << push(stk, &elems[3]) << '\n';

        process << "whether it is empty: " << empty(stk) << '\n';
        process << "size: " << size(stk) << '\n';
        process << "all details: " << Printer(&stk) << '\n';
        destroy(&stk);

        return std::ostringstream{process.str()};
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        using namespace date_structure::linear::queue;
        using date_structure::linear::Traverse;
        using Printer = Printer<Elem>;
        constexpr std::array<Elem, 4> elems = {'a', 'b', 'c', 'd'};
        std::ostringstream process{};

        Queue que{};
        create(&que);
        process << std::boolalpha;
        for (const auto& elem : elems)
        {
            process << "push " << elem << ": " << push(que, &elem) << '\n';
        }
        process << "traversal: ";
        Traverse(&que).order([&process](const void* const elem)
                             { process << *static_cast<const Elem*>(elem) << " ... "; });

        process << "\npop: " << *static_cast<Elem*>(pop(que)) << '\n';
        process << "front: " << *static_cast<Elem*>(front(que)) << '\n';
        process << "push " << elems[0] << ": " << push(que, elems.data()) << '\n';

        process << "whether it is empty: " << empty(que) << '\n';
        process << "size: " << size(que) << '\n';
        process << "all details: " << Printer(&que) << '\n';
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

//! @brief Alias for the key.
using Key = std::int16_t;
//! @brief Compare function for the key.
//! @param a - first key
//! @param b - second key
//! @return a is less than b if returns -1, a is greater than b if returns 1, and a is equal to b if returns 0
static int compareKey(const void* const a, const void* const b)
{
    const auto l = *static_cast<const Key*>(a), r = *static_cast<const Key*>(b);
    return (l > r) - (l < r);
}

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
        using Traverse = date_structure::tree::Traverse<BSTree, Node>;
        using Printer = date_structure::tree::Printer<Node, Key>;
        constexpr std::array<Key, 6> keys = {1, 5, 4, 3, 2, 6};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        BSTree* const tree = creation(&compareKey);
        const auto traverse = Traverse(tree);
        process << std::boolalpha;
        process << "insertion ";
        for (const auto& key : keys)
        {
            process << key << ", ";
            insertion(tree, &key);
        }
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        traverse.preOrder(opInTraversal);
        process << "\nin-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\npost-order traversal: ";
        traverse.postOrder(opInTraversal);

        process << "\nminimum: " << *static_cast<Key*>(getMinimum(tree)->key) << '\n';
        process << "maximum: " << *static_cast<Key*>(getMaximum(tree)->key) << '\n';
        process << "all details:\n" << Printer(tree->root);

        constexpr Key searchKey = 3;
        const auto* const searchNode = search(tree, &searchKey);
        process << "search " << searchKey << ": " << static_cast<bool>(searchNode) << '\n';
        process << "predecessor of " << searchKey << ": " << *static_cast<Key*>(getPredecessor(searchNode)->key)
                << '\n';
        process << "successor of " << searchKey << ": " << *static_cast<Key*>(getSuccessor(searchNode)->key) << '\n';
        process << "deletion " << searchKey << '\n';
        deletion(tree, &searchKey);

        process << "in-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\nall details:\n" << Printer(tree->root);
        destruction(tree);

        return std::ostringstream{process.str()};
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        using namespace date_structure::tree::avl;
        using Traverse = date_structure::tree::Traverse<AVLTree, Node>;
        using Printer = date_structure::tree::Printer<Node, Key>;
        constexpr std::array<Key, 16> keys = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        AVLTree* const tree = creation(&compareKey);
        const auto traverse = Traverse(tree);
        process << std::boolalpha;
        process << "height: " << getHeight(tree) << '\n';
        process << "insertion ";
        for (const auto& key : keys)
        {
            process << key << ", ";
            insertion(tree, &key);
        }
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        traverse.preOrder(opInTraversal);
        process << "\nin-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\npost-order traversal: ";
        traverse.postOrder(opInTraversal);

        process << "\nheight: " << getHeight(tree) << '\n';
        process << "minimum: " << *static_cast<Key*>(getMinimum(tree)->key) << '\n';
        process << "maximum: " << *static_cast<Key*>(getMaximum(tree)->key) << '\n';
        process << "all details:\n" << Printer(tree->root);
        constexpr Key searchKey = 13;
        const auto* const searchNode = search(tree, &searchKey);
        process << "search " << searchKey << ": " << static_cast<bool>(searchNode) << '\n';
        process << "deletion " << searchKey << '\n';
        deletion(tree, &searchKey);

        process << "height: " << getHeight(tree) << '\n';
        process << "in-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\nall details:\n" << Printer(tree->root);
        destruction(tree);

        return std::ostringstream{process.str()};
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        using namespace date_structure::tree::splay;
        using Traverse = date_structure::tree::Traverse<SplayTree, Node>;
        using Printer = date_structure::tree::Printer<Node, Key>;
        constexpr std::array<Key, 7> keys = {10, 50, 40, 70, 30, 20, 60};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        SplayTree* const tree = creation(&compareKey);
        const auto traverse = Traverse(tree);
        process << std::boolalpha;
        process << "insertion ";
        for (const auto& key : keys)
        {
            process << key << ", ";
            insertion(tree, &key);
        }
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        traverse.preOrder(opInTraversal);
        process << "\nin-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\npost-order traversal: ";
        traverse.postOrder(opInTraversal);

        process << "\nminimum: " << *static_cast<Key*>(getMinimum(tree)->key) << '\n';
        process << "maximum: " << *static_cast<Key*>(getMaximum(tree)->key) << '\n';
        process << "all details:\n" << Printer(tree->root);

        constexpr Key searchKey = 70;
        const auto* const searchNode = search(tree, &searchKey);
        process << "search " << searchKey << ": " << static_cast<bool>(searchNode) << '\n';
        process << "deletion " << searchKey << '\n';
        deletion(tree, &searchKey);

        process << "in-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\nall details:\n" << Printer(tree->root);

        constexpr Key splayKey = 30;
        process << "splaying " << splayKey << '\n';
        splaying(tree, &splayKey);

        process << "all details:\n" << Printer(tree->root);
        destruction(tree);

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
