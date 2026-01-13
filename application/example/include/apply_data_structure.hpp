//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif

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
const char* const version = data_structure::cache::version();

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
    //! @brief First in first out.
    //! @return procedure output
    static std::ostringstream fifo()
    {
        const KeyValue keyValueA = {'A', "foo"};
        const KeyValue keyValueB = {'B', "bar"};
        const KeyValue keyValueC = {'C', "baz"};
        const KeyValue keyValueD = {'D', "qux"};
        std::ostringstream process{};

        data_structure::cache::FIFO<Key, Value> fifoCache{3};
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
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        process << "\nerase range {B, C}: " << fifoCache.eraseRange(std::vector<Key>{'B', 'C'}) << '\n';
        auto resolvedRange =
            KeyOptValueRange{{'A', std::nullopt}, {'B', std::nullopt}, {'C', std::nullopt}, {'D', std::nullopt}};
        process << "resolve range " << resolvedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << ": ";
        fifoCache.resolveRange(resolvedRange);
        process << resolvedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        auto insertedRange = KeyValueRange{keyValueA, keyValueB, keyValueC, keyValueD};
        process << "\ninsert range " << insertedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        fifoCache.insertRange(std::move(insertedRange));

        process << "\nwhether it is empty: " << fifoCache.empty() << '\n';
        process << "size: " << fifoCache.size() << '\n';
        process << "capacity: " << fifoCache.capacity() << '\n';
        process << "current status: " << fifoCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        return std::ostringstream{process.str().substr(0, process.tellp()) + '\n'};
    }

    //! @brief Least frequently used.
    //! @return procedure output
    static std::ostringstream lfu()
    {
        const KeyValue keyValueA = {'A', "foo"};
        const KeyValue keyValueB = {'B', "bar"};
        const KeyValue keyValueC = {'C', "baz"};
        const KeyValue keyValueD = {'D', "qux"};
        std::ostringstream process{};

        data_structure::cache::LFU<Key, Value> lfuCache{3};
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
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        process << "\nerase range {B, C}: " << lfuCache.eraseRange(std::vector<Key>{'B', 'C'}) << '\n';
        auto resolvedRange =
            KeyOptValueRange{{'A', std::nullopt}, {'B', std::nullopt}, {'C', std::nullopt}, {'D', std::nullopt}};
        process << "resolve range " << resolvedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << ": ";
        lfuCache.resolveRange(resolvedRange);
        process << resolvedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        auto insertedRange = KeyValueRange{keyValueA, keyValueB, keyValueC, keyValueD};
        process << "\ninsert range " << insertedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        lfuCache.insertRange(std::move(insertedRange));

        process << "\nwhether it is empty: " << lfuCache.empty() << '\n';
        process << "size: " << lfuCache.size() << '\n';
        process << "capacity: " << lfuCache.capacity() << '\n';
        process << "current status: " << lfuCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        return std::ostringstream{process.str().substr(0, process.tellp()) + '\n'};
    }

    //! @brief Least recently used.
    //! @return procedure output
    static std::ostringstream lru()
    {
        const KeyValue keyValueA = {'A', "foo"};
        const KeyValue keyValueB = {'B', "bar"};
        const KeyValue keyValueC = {'C', "baz"};
        const KeyValue keyValueD = {'D', "qux"};
        std::ostringstream process{};

        data_structure::cache::LRU<Key, Value> lruCache{3};
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
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        process << "\nerase range {B, C}: " << lruCache.eraseRange(std::vector<Key>{'B', 'C'}) << '\n';
        auto resolvedRange =
            KeyOptValueRange{{'A', std::nullopt}, {'B', std::nullopt}, {'C', std::nullopt}, {'D', std::nullopt}};
        process << "resolve range " << resolvedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << ": ";
        lruCache.resolveRange(resolvedRange);
        process << resolvedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        auto insertedRange = KeyValueRange{keyValueA, keyValueB, keyValueC, keyValueD};
        process << "\ninsert range " << insertedRange;
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        lruCache.insertRange(std::move(insertedRange));

        process << "\nwhether it is empty: " << lruCache.empty() << '\n';
        process << "size: " << lruCache.size() << '\n';
        process << "capacity: " << lruCache.capacity() << '\n';
        process << "current status: " << lruCache.findRange(std::vector<Key>{'A', 'B', 'C', 'D'});
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        return std::ostringstream{process.str().substr(0, process.tellp()) + '\n'};
    }
};
} // namespace cache
extern void applyingCache(const std::vector<std::string>& candidates);

//! @brief Apply filter.
namespace filter
{
//! @brief The version used to apply.
const char* const version = data_structure::filter::version();

//! @brief Showcase for filter instances.
class Showcase
{
public:
    //! @brief Bloom.
    //! @return procedure output
    static std::ostringstream bloom()
    {
        constexpr std::uint32_t totalSize = 1000;
        const std::string keyPart1 = "foo://bar/";
        const std::string keyPart2 = "/baz.qux";
        std::ostringstream process{};
        std::vector<std::string> urls1{};
        std::vector<std::string> urls2{};
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

        data_structure::filter::Bloom bf(1e5, 1e-5, 0);
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
        const std::string keyPart1 = "foo://bar/";
        const std::string keyPart2 = "/baz.qux";
        std::ostringstream process{};
        std::vector<std::string> urls1{};
        std::vector<std::string> urls2{};
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

        data_structure::filter::Quotient qfA(16, 8, 0);
        data_structure::filter::Quotient qfB(16, 8, 0);
        data_structure::filter::Quotient qfC(16, 8, 0);
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
} // namespace filter
extern void applyingFilter(const std::vector<std::string>& candidates);

//! @brief Apply graph.
namespace graph
{
//! @brief The version used to apply.
const char* const version = data_structure::graph::version();

//! @brief Alias for the data.
using Data = char;
//! @brief Compare function for the data.
//! @param a - first data
//! @param b - second data
//! @return a is less than b if returns -1, a is greater than b if returns 1, and a is equal to b if returns 0
static int compareData(const void* const a, const void* const b)
{
    const auto l = *static_cast<const Data*>(a);
    const auto r = *static_cast<const Data*>(b);
    return static_cast<int>(l > r) - static_cast<int>(l < r);
}

//! @brief Showcase for graph instances.
class Showcase
{
public:
    // NOLINTBEGIN(readability-magic-numbers)
    //! @brief Undirected.
    //! @return procedure output
    static std::ostringstream undirected()
    {
        namespace undirected = data_structure::graph::undirected;
        using undirected::Traverse;
        constexpr std::array<Data, 7> vertices = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
        constexpr std::array<std::array<Data, 2>, 7> edges = {
            {{{'A', 'C'}}, {{'A', 'D'}}, {{'A', 'F'}}, {{'B', 'C'}}, {{'C', 'D'}}, {{'E', 'G'}}, {{'F', 'G'}}}};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const data)
        { process << *static_cast<const Data*>(data) << ' '; };

        undirected::AMLGraph* const graph = undirected::create(compareData);
        const auto traverse = Traverse(graph);
        process << std::boolalpha;
        for (const auto& vertex : vertices)
        {
            process << "add vertex " << vertex << ": " << undirected::addVertex(graph, &vertex) << '\n';
        }
        for (const auto& edge : edges)
        {
            process << "add edge " << edge[0] << '-' << edge[1] << ": "
                    << undirected::addEdge(graph, edge.data(), &edge[1]) << '\n';
        }

        process << "DFS traversal from " << vertices[0] << ": ";
        traverse.dfs(vertices.data(), opInTraversal);
        process << "\nBFS traversal from " << vertices[0] << ": ";
        traverse.bfs(vertices.data(), opInTraversal);

        process << "\ndelete edge " << edges[1][0] << '-' << edges[1][1] << ": "
                << undirected::deleteEdge(graph, edges[1].data(), &edges[1][1]) << '\n';
        process << "delete vertex " << vertices[0] << ": " << undirected::deleteVertex(graph, vertices.data()) << '\n';

        process << "DFS traversal from " << vertices[1] << ": ";
        traverse.dfs(&vertices[1], opInTraversal);
        process << "\nBFS traversal from " << vertices[1] << ": ";
        traverse.bfs(&vertices[1], opInTraversal);

        process << "\nDFS traversal from " << vertices[5] << ": ";
        traverse.dfs(&vertices[5], opInTraversal);
        process << "\nBFS traversal from " << vertices[5] << ": ";
        traverse.bfs(&vertices[5], opInTraversal);
        process << '\n';
        undirected::destroy(graph);
        return std::ostringstream{process.str()};
    }

    //! @brief Directed.
    //! @return procedure output
    static std::ostringstream directed()
    {
        namespace directed = data_structure::graph::directed;
        using directed::Traverse;
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

        directed::OLGraph* const graph = directed::create(compareData);
        const auto traverse = Traverse(graph);
        process << std::boolalpha;
        for (const auto& vertex : vertices)
        {
            process << "add vertex " << vertex << ": " << directed::addVertex(graph, &vertex) << '\n';
        }
        for (const auto& edge : edges)
        {
            process << "add arc " << edge[0] << '-' << edge[1] << ": " << directed::addArc(graph, edge.data(), &edge[1])
                    << '\n';
        }

        process << "DFS traversal from " << vertices[0] << ": ";
        traverse.dfs(vertices.data(), opInTraversal);
        process << "\nBFS traversal from " << vertices[0] << ": ";
        traverse.bfs(vertices.data(), opInTraversal);

        process << "\ndelete arc " << edges[6][0] << '-' << edges[6][1] << ": "
                << directed::deleteArc(graph, edges[6].data(), &edges[6][1]) << '\n';
        process << "delete vertex " << vertices[1] << ": " << directed::deleteVertex(graph, &vertices[1]) << '\n';

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
        directed::destroy(graph);
        return std::ostringstream{process.str()};
    }
    // NOLINTEND(readability-magic-numbers)
};
} // namespace graph
extern void applyingGraph(const std::vector<std::string>& candidates);

//! @brief Apply heap.
namespace heap
{
//! @brief The version used to apply.
const char* const version = data_structure::heap::version();

//! @brief Separator for each Key.
static constexpr std::string_view separator = ", ";
//! @brief Alias for the key.
using Key = std::int16_t;
//! @brief Compare function for the key.
//! @param a - first key
//! @param b - second key
//! @return a is less than b if returns -1, a is greater than b if returns 1, and a is equal to b if returns 0
static int compareKey(const void* const a, const void* const b)
{
    const auto l = *static_cast<const Key*>(a);
    const auto r = *static_cast<const Key*>(b);
    return static_cast<int>(l > r) - static_cast<int>(l < r);
}

//! @brief Showcase for heap instances.
class Showcase
{
public:
    //! @brief Binary.
    //! @return procedure output
    static std::ostringstream binary()
    {
        namespace binary = data_structure::heap::binary;
        constexpr std::array<Key, 9> keys = {80, 40, 30, 60, 90, 70, 10, 50, 20};
        constexpr int capacity = 30;
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        binary::BinaryHeap* const heap = binary::create(capacity, compareKey);
        const auto traverse = binary::Traverse(heap);
        process << std::boolalpha;
        process << "insert ";
        for (const auto& key : keys)
        {
            process << key << separator;
            binary::insert(heap, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << "\ntraversal: ";
        traverse.order(opInTraversal);

        constexpr Key insertKey = 15;
        process << "\ninsert " << insertKey << ": " << binary::insert(heap, &insertKey) << '\n';
        process << "traversal: ";
        traverse.order(opInTraversal);

        constexpr Key removeKey = 10;
        process << "\nremove " << removeKey << ": " << binary::remove(heap, &removeKey) << '\n';
        process << "traversal: ";
        traverse.order(opInTraversal);
        process << '\n';
        binary::destroy(heap);
        return std::ostringstream{process.str()};
    }

    //! @brief Leftist.
    //! @return procedure output
    static std::ostringstream leftist()
    {
        namespace leftist = data_structure::heap::leftist;
        using Traverse = data_structure::heap::Traverse<leftist::LeftistHeap, leftist::Node>;
        using Printer = data_structure::heap::Printer<leftist::Node, Key>;
        constexpr std::array<Key, 8> keys1 = {10, 40, 24, 30, 36, 20, 12, 16};
        constexpr std::array<Key, 7> keys2 = {17, 13, 11, 15, 19, 21, 23};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        leftist::LeftistHeap* const heapA = leftist::create(compareKey);
        const auto traverseA = Traverse(heapA);
        process << std::boolalpha;
        process << "A insert ";
        for (const auto& key : keys1)
        {
            process << key << separator;
            leftist::insert(heapA, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << "\nA all details:\n" << Printer(heapA->root);

        leftist::LeftistHeap* const heapB = leftist::create(compareKey);
        process << "B insert ";
        for (const auto& key : keys2)
        {
            process << key << separator;
            leftist::insert(heapB, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << "\nB all details:\n" << Printer(heapB->root);

        process << "A merge B\n";
        leftist::merge(heapA, heapB);
        process << "A pre-order traversal: ";
        traverseA.preOrder(opInTraversal);
        process << "\nA in-order traversal: ";
        traverseA.inOrder(opInTraversal);
        process << "\nA post-order traversal: ";
        traverseA.postOrder(opInTraversal);

        process << "\nA minimum: " << *static_cast<Key*>(leftist::getMinimum(heapA)) << '\n';
        process << "A remove\n";
        remove(heapA);
        process << "A all details:\n" << Printer(heapA->root);
        leftist::destroy(heapA);
        leftist::destroy(heapB);
        return std::ostringstream{process.str()};
    }

    //! @brief Skew.
    //! @return procedure output
    static std::ostringstream skew()
    {
        namespace skew = data_structure::heap::skew;
        using Traverse = data_structure::heap::Traverse<skew::SkewHeap, skew::Node>;
        using Printer = data_structure::heap::Printer<skew::Node, Key>;
        constexpr std::array<Key, 8> keys1 = {10, 40, 24, 30, 36, 20, 12, 16};
        constexpr std::array<Key, 7> keys2 = {17, 13, 11, 15, 19, 21, 23};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        skew::SkewHeap* const heapA = skew::create(compareKey);
        const auto traverseA = Traverse(heapA);
        process << std::boolalpha;
        process << "A insert ";
        for (const auto& key : keys1)
        {
            process << key << separator;
            skew::insert(heapA, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << "\nA all details:\n" << Printer(heapA->root);

        skew::SkewHeap* const heapB = skew::create(compareKey);
        process << "B insert ";
        for (const auto& key : keys2)
        {
            process << key << separator;
            skew::insert(heapB, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));
        process << "\nB all details:\n" << Printer(heapB->root);

        process << "A merge B\n";
        skew::merge(heapA, heapB);
        process << "A pre-order traversal: ";
        traverseA.preOrder(opInTraversal);
        process << "\nA in-order traversal: ";
        traverseA.inOrder(opInTraversal);
        process << "\nA post-order traversal: ";
        traverseA.postOrder(opInTraversal);

        process << "\nA minimum: " << *static_cast<Key*>(skew::getMinimum(heapA)) << '\n';
        process << "A remove\n";
        remove(heapA);
        process << "A all details:\n" << Printer(heapA->root);
        skew::destroy(heapA);
        skew::destroy(heapB);
        return std::ostringstream{process.str()};
    }
};
} // namespace heap
extern void applyingHeap(const std::vector<std::string>& candidates);

//! @brief Apply linear.
namespace linear
{
//! @brief The version used to apply.
const char* const version = data_structure::linear::version();

//! @brief Alias for the value.
using Value = std::int16_t;

//! @brief Showcase for linear instances.
class Showcase
{
public:
    //! @brief Doubly linked list.
    //! @return procedure output
    static std::ostringstream dll()
    {
        namespace dll = data_structure::linear::dll;
        using data_structure::linear::Traverse;
        using Printer = dll::Printer<Value>;
        constexpr std::array<Value, 4> values = {'a', 'b', 'c', 'd'};
        std::ostringstream process{};

        dll::DLL linear{};
        dll::create(&linear);
        process << std::boolalpha;
        for (const auto& value : values)
        {
            process << "insert (0) " << value << ": " << dll::insert(linear, 0, &value) << '\n';
        }
        process << "traversal: ";
        Traverse(&linear).order([&process](const void* const value)
                                { process << *static_cast<const Value*>(value) << " ... "; });

        process << "\nremove (1): " << dll::remove(linear, 1) << '\n';
        process << "insert (2) " << values[2] << ": " << dll::insert(linear, 2, &values[2]) << '\n';

        process << "insert first " << values[0] << ": " << dll::insertFirst(linear, values.data()) << '\n';
        process << "insert last " << values[3] << ": " << dll::insertLast(linear, &values[3]) << '\n';
        process << "get first: " << *static_cast<Value*>(dll::getFirst(linear)) << '\n';
        process << "get last: " << *static_cast<Value*>(dll::getLast(linear)) << '\n';
        process << "remove first: " << dll::removeFirst(linear) << '\n';
        process << "remove last: " << dll::removeLast(linear) << '\n';

        process << "whether it is empty: " << dll::empty(linear) << '\n';
        process << "size: " << dll::size(linear) << '\n';
        process << "all details: " << Printer(&linear) << '\n';
        dll::destroy(&linear);
        return std::ostringstream{process.str()};
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        namespace stack = data_structure::linear::stack;
        using data_structure::linear::Traverse;
        using Printer = stack::Printer<Value>;
        constexpr std::array<Value, 4> values = {'a', 'b', 'c', 'd'};
        std::ostringstream process{};

        stack::Stack linear{};
        stack::create(&linear);
        process << std::boolalpha;
        for (const auto& value : values)
        {
            process << "push " << value << ": " << stack::push(linear, &value) << '\n';
        }
        process << "traversal: ";
        Traverse(&linear).order([&process](const void* const value)
                                { process << *static_cast<const Value*>(value) << " ... "; });

        process << "\npop: " << *static_cast<Value*>(stack::pop(linear)) << '\n';
        process << "top: " << *static_cast<Value*>(stack::top(linear)) << '\n';
        process << "push " << values[3] << ": " << stack::push(linear, &values[3]) << '\n';

        process << "whether it is empty: " << stack::empty(linear) << '\n';
        process << "size: " << stack::size(linear) << '\n';
        process << "all details: " << Printer(&linear) << '\n';
        stack::destroy(&linear);
        return std::ostringstream{process.str()};
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        namespace queue = data_structure::linear::queue;
        using data_structure::linear::Traverse;
        using Printer = queue::Printer<Value>;
        constexpr std::array<Value, 4> values = {'a', 'b', 'c', 'd'};
        std::ostringstream process{};

        queue::Queue linear{};
        queue::create(&linear);
        process << std::boolalpha;
        for (const auto& value : values)
        {
            process << "push " << value << ": " << queue::push(linear, &value) << '\n';
        }
        process << "traversal: ";
        Traverse(&linear).order([&process](const void* const value)
                                { process << *static_cast<const Value*>(value) << " ... "; });

        process << "\npop: " << *static_cast<Value*>(queue::pop(linear)) << '\n';
        process << "front: " << *static_cast<Value*>(queue::front(linear)) << '\n';
        process << "push " << values[0] << ": " << queue::push(linear, values.data()) << '\n';

        process << "whether it is empty: " << queue::empty(linear) << '\n';
        process << "size: " << queue::size(linear) << '\n';
        process << "all details: " << Printer(&linear) << '\n';
        queue::destroy(&linear);
        return std::ostringstream{process.str()};
    }
};
} // namespace linear
extern void applyingLinear(const std::vector<std::string>& candidates);

//! @brief Apply tree.
namespace tree
{
//! @brief The version used to apply.
const char* const version = data_structure::tree::version();

//! @brief Separator for each Key.
static constexpr std::string_view separator = ", ";
//! @brief Alias for the key.
using Key = std::int16_t;
//! @brief Compare function for the key.
//! @param a - first key
//! @param b - second key
//! @return a is less than b if returns -1, a is greater than b if returns 1, and a is equal to b if returns 0
static int compareKey(const void* const a, const void* const b)
{
    const auto l = *static_cast<const Key*>(a);
    const auto r = *static_cast<const Key*>(b);
    return static_cast<int>(l > r) - static_cast<int>(l < r);
}

//! @brief Showcase for tree instances.
class Showcase
{
public:
    //! @brief Binary search.
    //! @return procedure output
    static std::ostringstream bs()
    {
        namespace bs = data_structure::tree::bs;
        using Traverse = data_structure::tree::Traverse<bs::BSTree, bs::Node>;
        using Printer = data_structure::tree::Printer<bs::Node, Key>;
        constexpr std::array<Key, 6> keys = {1, 5, 4, 3, 2, 6};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        bs::BSTree* const tree = bs::create(compareKey);
        const auto traverse = Traverse(tree);
        process << std::boolalpha;
        process << "insert ";
        for (const auto& key : keys)
        {
            process << key << separator;
            bs::insert(tree, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        process << "\npre-order traversal: ";
        traverse.preOrder(opInTraversal);
        process << "\nin-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\npost-order traversal: ";
        traverse.postOrder(opInTraversal);

        process << "\nminimum: " << *static_cast<Key*>(bs::getMinimum(tree)->key) << '\n';
        process << "maximum: " << *static_cast<Key*>(bs::getMaximum(tree)->key) << '\n';
        process << "all details:\n" << Printer(tree->root);

        constexpr Key searchKey = 3;
        const auto* const searchNode = bs::search(tree, &searchKey);
        process << "search " << searchKey << ": " << static_cast<bool>(searchNode) << '\n';
        process << "predecessor of " << searchKey << ": " << *static_cast<Key*>(bs::getPredecessor(searchNode)->key)
                << '\n';
        process << "successor of " << searchKey << ": " << *static_cast<Key*>(bs::getSuccessor(searchNode)->key)
                << '\n';
        process << "remove " << searchKey << '\n';
        bs::remove(tree, &searchKey);

        process << "in-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\nall details:\n" << Printer(tree->root);
        bs::destroy(tree);
        return std::ostringstream{process.str()};
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        namespace avl = data_structure::tree::avl;
        using Traverse = data_structure::tree::Traverse<avl::AVLTree, avl::Node>;
        using Printer = data_structure::tree::Printer<avl::Node, Key>;
        constexpr std::array<Key, 16> keys = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        avl::AVLTree* const tree = avl::create(compareKey);
        const auto traverse = Traverse(tree);
        process << std::boolalpha;
        process << "height: " << avl::getHeight(tree) << '\n';
        process << "insert ";
        for (const auto& key : keys)
        {
            process << key << separator;
            avl::insert(tree, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        process << "\npre-order traversal: ";
        traverse.preOrder(opInTraversal);
        process << "\nin-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\npost-order traversal: ";
        traverse.postOrder(opInTraversal);

        process << "\nheight: " << avl::getHeight(tree) << '\n';
        process << "minimum: " << *static_cast<Key*>(avl::getMinimum(tree)->key) << '\n';
        process << "maximum: " << *static_cast<Key*>(avl::getMaximum(tree)->key) << '\n';
        process << "all details:\n" << Printer(tree->root);
        constexpr Key searchKey = 13;
        const auto* const searchNode = avl::search(tree, &searchKey);
        process << "search " << searchKey << ": " << static_cast<bool>(searchNode) << '\n';
        process << "remove " << searchKey << '\n';
        avl::remove(tree, &searchKey);

        process << "height: " << avl::getHeight(tree) << '\n';
        process << "in-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\nall details:\n" << Printer(tree->root);
        avl::destroy(tree);
        return std::ostringstream{process.str()};
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        namespace splay = data_structure::tree::splay;
        using Traverse = data_structure::tree::Traverse<splay::SplayTree, splay::Node>;
        using Printer = data_structure::tree::Printer<splay::Node, Key>;
        constexpr std::array<Key, 7> keys = {10, 50, 40, 70, 30, 20, 60};
        std::ostringstream process{};
        const auto opInTraversal = [&process](const void* const key)
        { process << *static_cast<const Key*>(key) << " ... "; };

        splay::SplayTree* const tree = splay::create(compareKey);
        const auto traverse = Traverse(tree);
        process << std::boolalpha;
        process << "insert ";
        for (const auto& key : keys)
        {
            process << key << separator;
            splay::insert(tree, &key);
        }
        process.seekp(static_cast<std::streamoff>(process.str().length() - separator.length()));

        process << "\npre-order traversal: ";
        traverse.preOrder(opInTraversal);
        process << "\nin-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\npost-order traversal: ";
        traverse.postOrder(opInTraversal);

        process << "\nminimum: " << *static_cast<Key*>(splay::getMinimum(tree)->key) << '\n';
        process << "maximum: " << *static_cast<Key*>(splay::getMaximum(tree)->key) << '\n';
        process << "all details:\n" << Printer(tree->root);

        constexpr Key searchKey = 70;
        const auto* const searchNode = splay::search(tree, &searchKey);
        process << "search " << searchKey << ": " << static_cast<bool>(searchNode) << '\n';
        process << "remove " << searchKey << '\n';
        splay::remove(tree, &searchKey);

        process << "in-order traversal: ";
        traverse.inOrder(opInTraversal);
        process << "\nall details:\n" << Printer(tree->root);

        constexpr Key splayKey = 30;
        process << "splay " << splayKey << '\n';
        splay::splay(tree, &splayKey);

        process << "all details:\n" << Printer(tree->root);
        splay::destroy(tree);
        return std::ostringstream{process.str()};
    }
};
} // namespace tree
extern void applyingTree(const std::vector<std::string>& candidates);
} // namespace app_ds
} // namespace application
