//! @file graph.hpp
//! @author ryftchen
//! @brief The declarations (graph) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>

//! @brief The data structure module.
namespace date_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Graph-related functions in the data structure module.
namespace graph
{
extern const char* version() noexcept;

//! @brief The undirected graph structure.
namespace undirected
{
//! @brief The maximum number of vertices in the graph.
constexpr int maxVertexNum = 128;
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The edge structure in the graph.
    typedef struct TagEdge
    {
        //! @brief The one endpoint vertex index in the edge.
        int iVex;
        //! @brief The other endpoint vertex index in the edge.
        int jVex;
        //! @brief Pointer to the next edge in the adjacency list of the one endpoint vertex.
        struct TagEdge* iLink;
        //! @brief Pointer to the next edge in the adjacency list of the other endpoint vertex.
        struct TagEdge* jLink;
    } EdgeNode;

    //! @brief The vertex structure in the graph.
    typedef struct TagVertex
    {
        //! @brief Pointer to the first incident edge in the adjacency list.
        struct TagEdge* firstEdge;
        //! @brief Vertex data.
        void* data;
    } VertexNode;

    //! @brief The undirected graph structure using an adjacency multi-list.
    typedef struct TagGraph
    {
        //! @brief Number of vertices.
        int vexNum;
        //! @brief Number of edges.
        int edgeNum;
        //! @brief Vertex list in the adjacency multi-list representation.
        struct TagVertex adjMultiList[maxVertexNum];
        //! @brief The data's compare function.
        Compare compare;
    } AMLGraph;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern void create(AMLGraph* const graph, const Compare cmp);
extern void destroy(AMLGraph* const graph);
extern bool insertVertex(AMLGraph* const graph, const void* const vert);
extern bool insertEdge(AMLGraph* const graph, const void* const vert1, const void* const vert2);
extern bool deleteVertex(AMLGraph* const graph, const void* const vert);
extern bool deleteEdge(AMLGraph* const graph, const void* const vert1, const void* const vert2);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param graph - graph to be traversed
    explicit Traverse(const AMLGraph* const graph) : graph{graph} {}
    //! @brief Destroy the Traverse object.
    virtual ~Traverse() = default;

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
    //! @brief The graph to be traversed.
    const AMLGraph* const graph{nullptr};
    //! @brief Perform a depth-first search (DFS) traversal recursively.
    //! @param index - index of the starting vertex
    //! @param visited - to track visited vertices
    //! @param op - operation on each vertex
    void dfsRecursive(const int index, bool visited[], const Operation& op) const;
    //! @brief Sort the neighbors of a vertex based on their data in ascending order.
    //! @param neighbors - neighbor indices
    //! @param size - number of neighbors
    void sortNeighbors(int neighbors[], const int size) const;
};
} // namespace undirected

//! @brief The directed graph structure.
namespace directed
{
//! @brief The maximum number of vertices in the graph.
constexpr int maxVertexNum = 128;
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The arc structure in the graph.
    typedef struct TagArc
    {
        //! @brief The head vertex index of the arc.
        int headVex;
        //! @brief The tail vertex index of the arc.
        int tailVex;
        //! @brief Pointer to the next arc in the incoming arcs list.
        struct TagArc* headLink;
        //! @brief Pointer to the next arc in the outgoing arcs list.
        struct TagArc* tailLink;
    } ArcNode;

    //! @brief The vertex structure in the graph.
    typedef struct TagVertex
    {
        //! @brief Pointer to the first incoming arc.
        struct TagArc* firstIn;
        //! @brief Pointer to the first outgoing arc.
        struct TagArc* firstOut;
        //! @brief Vertex data.
        void* data;
    } VertexNode;

    //! @brief The directed graph structure using an orthogonal linked list.
    typedef struct TagGraph
    {
        //! @brief Number of vertices.
        int vexNum;
        //! @brief Number of arcs.
        int arcNum;
        //! @brief Vertex list in the orthogonal linked list representation.
        struct TagVertex xList[maxVertexNum];
        //! @brief The data's compare function.
        Compare compare;
    } OLGraph;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern void create(OLGraph* const graph, const Compare cmp);
extern void destroy(OLGraph* const graph);
extern bool insertVertex(OLGraph* const graph, const void* const vert);
extern bool insertArc(OLGraph* const graph, const void* const vert1, const void* const vert2);
extern bool deleteVertex(OLGraph* const graph, const void* const vert);
extern bool deleteArc(OLGraph* const graph, const void* const vert1, const void* const vert2);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param graph - graph to be traversed
    explicit Traverse(const OLGraph* const graph) : graph{graph} {}
    //! @brief Destroy the Traverse object.
    virtual ~Traverse() = default;

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
    //! @brief The graph to be traversed.
    const OLGraph* const graph{nullptr};
    //! @brief Perform a depth-first search (DFS) traversal recursively.
    //! @param index - index of the starting vertex
    //! @param visited - to track visited vertices
    //! @param op - operation on each vertex
    void dfsRecursive(const int index, bool visited[], const Operation& op) const;
    //! @brief Sort the neighbors of a vertex based on their data in ascending order.
    //! @param neighbors - neighbor indices
    //! @param size - number of neighbors
    void sortNeighbors(int neighbors[], const int size) const;
};
} // namespace directed
} // namespace graph
} // namespace date_structure
