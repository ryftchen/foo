//! @file graph.hpp
//! @author ryftchen
//! @brief The declarations (graph) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>

//! @brief The data structure module.
namespace data_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Graph-related functions in the data structure module.
namespace graph
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "DS_GRAPH";
}
extern const char* version() noexcept;

//! @brief The undirected graph structure.
namespace undirected
{
//! @brief The maximum number of vertices in the graph.
constexpr int maxVertexNum = 256;
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The edge structure in the graph.
    struct EdgeNode
    {
        //! @brief The one endpoint vertex index in the edge.
        int iVex;
        //! @brief The other endpoint vertex index in the edge.
        int jVex;
        //! @brief Pointer to the next edge in the adjacency list of the one endpoint vertex.
        struct EdgeNode* iLink;
        //! @brief Pointer to the next edge in the adjacency list of the other endpoint vertex.
        struct EdgeNode* jLink;
    };

    //! @brief The vertex structure in the graph.
    struct VertexNode
    {
        //! @brief Pointer to the first incident edge in the adjacency list.
        struct EdgeNode* firstEdge;
        //! @brief Vertex data.
        void* data;
    };

    //! @brief The undirected graph structure using an adjacency multi-list.
    struct AMLGraph
    {
        //! @brief Number of vertices.
        int vexNum;
        //! @brief Number of edges.
        int edgeNum;
        //! @brief Vertex list in the adjacency multi-list representation.
        struct VertexNode adjMultiList[maxVertexNum];
        //! @brief The data's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern AMLGraph* create(const Compare cmp);
extern void destroy(AMLGraph* graph);
extern bool addVertex(AMLGraph* const graph, const void* const vert);
extern bool addEdge(AMLGraph* const graph, const void* const vert1, const void* const vert2);
extern bool deleteVertex(AMLGraph* const graph, const void* const vert);
extern bool deleteEdge(AMLGraph* const graph, const void* const vert1, const void* const vert2);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param graph - graph structure to be traversed
    explicit Traverse(const AMLGraph* const graph) : graph{graph} {}

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a breadth-first search (BFS) traversal starting from a given vertex.
    //! @param vert - starting vertex
    //! @param op - operation on each vertex
    void dfs(const void* const vert, const Operation& op) const;
    //! @brief Perform a depth-first search (DFS) traversal starting from a given vertex.
    //! @param vert - starting vertex
    //! @param op - operation on each vertex
    void bfs(const void* const vert, const Operation& op) const;

private:
    //! @brief The graph structure to be traversed.
    const AMLGraph* const graph{nullptr};
    //! @brief Perform a depth-first search (DFS) traversal recursively.
    //! @param index - index of the starting vertex
    //! @param visited - track visited vertices
    //! @param op - operation on each vertex
    void dfsRecursive(const int index, std::array<bool, maxVertexNum>& visited, const Operation& op) const;
    //! @brief Sort the neighbors of a vertex based on their data in ascending order.
    //! @param neighbors - neighbor indices
    //! @param size - number of neighbors
    void sortNeighbors(std::array<int, maxVertexNum>& neighbors, const int size) const;
};
} // namespace undirected

//! @brief The directed graph structure.
namespace directed
{
//! @brief The maximum number of vertices in the graph.
constexpr int maxVertexNum = 256;
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The arc structure in the graph.
    struct ArcNode
    {
        //! @brief The head vertex index of the arc.
        int headVex;
        //! @brief The tail vertex index of the arc.
        int tailVex;
        //! @brief Pointer to the next arc in the incoming arcs list.
        struct ArcNode* headLink;
        //! @brief Pointer to the next arc in the outgoing arcs list.
        struct ArcNode* tailLink;
    };

    //! @brief The vertex structure in the graph.
    struct VertexNode
    {
        //! @brief Pointer to the first incoming arc.
        struct ArcNode* firstIn;
        //! @brief Pointer to the first outgoing arc.
        struct ArcNode* firstOut;
        //! @brief Vertex data.
        void* data;
    };

    //! @brief The directed graph structure using an orthogonal linked list.
    struct OLGraph
    {
        //! @brief Number of vertices.
        int vexNum;
        //! @brief Number of arcs.
        int arcNum;
        //! @brief Vertex list in the orthogonal linked list representation.
        struct VertexNode xList[maxVertexNum];
        //! @brief The data's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern OLGraph* create(const Compare cmp);
extern void destroy(OLGraph* graph);
extern bool addVertex(OLGraph* const graph, const void* const vert);
extern bool addArc(OLGraph* const graph, const void* const vert1, const void* const vert2);
extern bool deleteVertex(OLGraph* const graph, const void* const vert);
extern bool deleteArc(OLGraph* const graph, const void* const vert1, const void* const vert2);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param graph - graph structure to be traversed
    explicit Traverse(const OLGraph* const graph) : graph{graph} {}

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a breadth-first search (BFS) traversal starting from a given vertex.
    //! @param vert - starting vertex
    //! @param op - operation on each vertex
    void dfs(const void* const vert, const Operation& op) const;
    //! @brief Perform a depth-first search (DFS) traversal starting from a given vertex.
    //! @param vert - starting vertex
    //! @param op - operation on each vertex
    void bfs(const void* const vert, const Operation& op) const;

private:
    //! @brief The graph structure to be traversed.
    const OLGraph* const graph{nullptr};
    //! @brief Perform a depth-first search (DFS) traversal recursively.
    //! @param index - index of the starting vertex
    //! @param visited - track visited vertices
    //! @param op - operation on each vertex
    void dfsRecursive(const int index, std::array<bool, maxVertexNum>& visited, const Operation& op) const;
    //! @brief Sort the neighbors of a vertex based on their data in ascending order.
    //! @param neighbors - neighbor indices
    //! @param size - number of neighbors
    void sortNeighbors(std::array<int, maxVertexNum>& neighbors, const int size) const;
};
} // namespace directed
} // namespace graph
} // namespace data_structure
