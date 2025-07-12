//! @file graph.cpp
//! @author ryftchen
//! @brief The definitions (graph) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "graph.hpp"

namespace date_structure::graph
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
namespace undirected
{
//! @brief Locate the index of a vertex in the undirected graph.
//! @param graph - graph to search in
//! @param vert - vertex to locate
//! @return index of the vertex if found, otherwise -1
static int locateVertex(const AMLGraph* const graph, const void* const vert)
{
    if (!graph->compare)
    {
        return -1;
    }

    for (int i = 0; i < graph->vexNum; ++i)
    {
        if (graph->compare(graph->adjMultiList[i].data, vert) == 0)
        {
            return i;
        }
    }

    return -1;
}

//! @brief Remove the given edge from a vertex's adjacency list in the undirected graph.
//! @param vNode - vertex node whose adjacency list will be modified
//! @param index - index of the vertex
//! @param eNode - edge node to be removed
//! @return success or failure
static bool removeEdgeFromList(VertexNode* const vNode, const int index, const EdgeNode* const eNode)
{
    EdgeNode *curr = vNode->firstEdge, *prev = nullptr;
    while (curr)
    {
        if (curr != eNode)
        {
            prev = curr;
            curr = (curr->iVex == index) ? curr->iLink : curr->jLink;
            continue;
        }

        if (!prev)
        {
            vNode->firstEdge = (curr->iVex == index) ? curr->iLink : curr->jLink;
        }
        else if (prev->iVex == index)
        {
            prev->iLink = (curr->iVex == index) ? curr->iLink : curr->jLink;
        }
        else
        {
            prev->jLink = (curr->iVex == index) ? curr->iLink : curr->jLink;
        }
        return true;
    }

    return false;
}

//! @brief Create the undirected graph.
//! @param graph - graph to create
//! @param cmp - compare function to compare data
void create(AMLGraph* const graph, const Compare cmp)
{
    graph->vexNum = 0;
    graph->edgeNum = 0;
    graph->compare = cmp;
}

//! @brief Destroy the undirected graph.
//! @param graph - graph to destroy
void destroy(AMLGraph* const graph)
{
    if (!graph)
    {
        return;
    }

    const EdgeNode *curr = nullptr, *del = nullptr;
    for (int i = 0; i < graph->vexNum; ++i)
    {
        curr = graph->adjMultiList[i].firstEdge;
        while (curr)
        {
            del = curr;
            curr = (curr->iVex == i) ? curr->iLink : curr->jLink;
            deleteEdge(graph, graph->adjMultiList[del->iVex].data, graph->adjMultiList[del->jVex].data);
        }
    }

    graph->vexNum = 0;
    graph->edgeNum = 0;
}

//! @brief Insert a vertex into the undirected graph.
//! @param graph - graph to insert into
//! @param vert - vert to insert
//! @return success or failure
bool insertVertex(AMLGraph* const graph, const void* const vert)
{
    if (!graph || (graph->vexNum >= maxVertexNum) || (locateVertex(graph, vert) >= 0))
    {
        return false;
    }

    graph->adjMultiList[graph->vexNum].firstEdge = nullptr;
    graph->adjMultiList[graph->vexNum].data = const_cast<void*>(vert);
    ++graph->vexNum;

    return true;
}

//! @brief Insert an edge between two vertices into the undirected graph.
//! @param graph - graph to insert into
//! @param vert1 - vertex at the one endpoint of the edge
//! @param vert2 - vertex at the other endpoint of the edge
//! @return success or failure
bool insertEdge(AMLGraph* const graph, const void* const vert1, const void* const vert2)
{
    if (!graph)
    {
        return false;
    }

    const int index1 = locateVertex(graph, vert1), index2 = locateVertex(graph, vert2);
    if ((index1 < 0) || (index2 < 0))
    {
        return false;
    }

    const EdgeNode* curr = graph->adjMultiList[index1].firstEdge;
    while (curr)
    {
        if (((curr->iVex == index1) && (curr->jVex == index2)) || ((curr->iVex == index2) && (curr->jVex == index1)))
        {
            return false;
        }
        curr = (curr->iVex == index1) ? curr->iLink : curr->jLink;
    }

    auto* const newNode = ::new (std::nothrow) EdgeNode;
    newNode->iVex = index1;
    newNode->jVex = index2;
    newNode->iLink = graph->adjMultiList[index1].firstEdge;
    graph->adjMultiList[index1].firstEdge = newNode;
    newNode->jLink = graph->adjMultiList[index2].firstEdge;
    graph->adjMultiList[index2].firstEdge = newNode;
    ++graph->edgeNum;

    return true;
}

//! @brief Delete a vertex from the undirected graph.
//! @param graph - graph to delete from
//! @param vert - vert to delete
//! @return success or failure
bool deleteVertex(AMLGraph* const graph, const void* const vert)
{
    if (!graph)
    {
        return false;
    }

    const int index = locateVertex(graph, vert);
    if (index < 0)
    {
        return false;
    }

    const EdgeNode* curr = graph->adjMultiList[index].firstEdge;
    while (curr)
    {
        const void* const other =
            (curr->iVex == index) ? graph->adjMultiList[curr->jVex].data : graph->adjMultiList[curr->iVex].data;
        deleteEdge(graph, vert, other);
        curr = graph->adjMultiList[index].firstEdge;
    }

    if (index == (graph->vexNum - 1))
    {
        --graph->vexNum;
        return true;
    }

    graph->adjMultiList[index] = graph->adjMultiList[graph->vexNum - 1];
    for (int i = 0; i < graph->vexNum; ++i)
    {
        EdgeNode* adj = graph->adjMultiList[i].firstEdge;
        while (adj)
        {
            if (adj->iVex == (graph->vexNum - 1))
            {
                adj->iVex = index;
            }
            if (adj->jVex == (graph->vexNum - 1))
            {
                adj->jVex = index;
            }
            adj = (adj->iVex == i) ? adj->iLink : adj->jLink;
        }
    }
    --graph->vexNum;

    return true;
}

//! @brief Delete an edge between two vertices from the undirected graph.
//! @param graph - graph to delete from
//! @param vert1 - vertex at the one endpoint of the edge
//! @param vert2 - vertex at the other endpoint of the edge
//! @return success or failure
bool deleteEdge(AMLGraph* const graph, const void* const vert1, const void* const vert2)
{
    if (!graph)
    {
        return false;
    }

    const int index1 = locateVertex(graph, vert1), index2 = locateVertex(graph, vert2);
    if ((index1 < 0) || (index2 < 0))
    {
        return false;
    }

    const EdgeNode* curr = graph->adjMultiList[index1].firstEdge;
    while (curr)
    {
        if (((curr->iVex == index1) && (curr->jVex == index2)) || ((curr->iVex == index2) && (curr->jVex == index1)))
        {
            break;
        }
        curr = (curr->iVex == index1) ? curr->iLink : curr->jLink;
    }

    if (!curr)
    {
        return false;
    }

    removeEdgeFromList(&graph->adjMultiList[index1], index1, curr);
    removeEdgeFromList(&graph->adjMultiList[index2], index2, curr);

    ::delete curr;
    curr = nullptr;
    --graph->edgeNum;

    return true;
}

void Traverse::dfs(const void* const vert, const Operation& op) const
{
    if (!graph)
    {
        return;
    }

    const int start = locateVertex(graph, vert);
    if (start < 0)
    {
        return;
    }

    bool visited[maxVertexNum] = {};
    dfsRecursive(start, visited, op);
}

void Traverse::bfs(const void* const vert, const Operation& op) const
{
    if (!graph)
    {
        return;
    }

    const int start = locateVertex(graph, vert);
    if (start < 0)
    {
        return;
    }

    bool visited[maxVertexNum] = {};
    visited[start] = true;
    op(graph->adjMultiList[start].data);

    int queue[maxVertexNum] = {}, front = 0, rear = 0;
    queue[rear++] = start;
    while (front < rear)
    {
        const int q = queue[front++];
        int neighbors[maxVertexNum] = {}, counter = 0;
        const EdgeNode* curr = graph->adjMultiList[q].firstEdge;
        while (curr)
        {
            const int nbrIndex = (curr->iVex == q) ? curr->jVex : curr->iVex;
            if (!visited[nbrIndex])
            {
                neighbors[counter++] = nbrIndex;
            }
            curr = (curr->iVex == q) ? curr->iLink : curr->jLink;
        }

        sortNeighbors(neighbors, counter);
        for (int i = 0; i < counter; ++i)
        {
            if (!visited[neighbors[i]])
            {
                visited[neighbors[i]] = true;
                op(graph->adjMultiList[neighbors[i]].data);
                queue[rear++] = neighbors[i];
            }
        }
    }
}

void Traverse::dfsRecursive(const int index, bool visited[], const Operation& op) const
{
    visited[index] = true;
    op(graph->adjMultiList[index].data);

    int neighbors[maxVertexNum] = {}, counter = 0;
    const EdgeNode* curr = graph->adjMultiList[index].firstEdge;
    while (curr)
    {
        const int nbrIndex = (curr->iVex == index) ? curr->jVex : curr->iVex;
        if (!visited[nbrIndex])
        {
            neighbors[counter++] = nbrIndex;
        }
        curr = (curr->iVex == index) ? curr->iLink : curr->jLink;
    }

    sortNeighbors(neighbors, counter);
    for (int i = 0; i < counter; ++i)
    {
        if (!visited[neighbors[i]])
        {
            dfsRecursive(neighbors[i], visited, op);
        }
    }
}

void Traverse::sortNeighbors(int neighbors[], const int size) const
{
    if (!graph->compare)
    {
        return;
    }

    for (int i = 1; i < size; ++i)
    {
        const int temp = neighbors[i];
        int j = i - 1;
        while ((j >= 0) && (graph->compare(graph->adjMultiList[neighbors[j]].data, graph->adjMultiList[temp].data) > 0))
        {
            neighbors[j + 1] = neighbors[j];
            --j;
        }
        neighbors[j + 1] = temp;
    }
}
} // namespace undirected

namespace directed
{
//! @brief Locate the index of a vertex in the directed graph.
//! @param graph - graph to search in
//! @param vert - vertex to locate
//! @return index of the vertex if found, otherwise -1
static int locateVertex(const OLGraph* const graph, const void* const vert)
{
    if (!graph->compare)
    {
        return -1;
    }

    for (int i = 0; i < graph->vexNum; ++i)
    {
        if (graph->compare(graph->xList[i].data, vert) == 0)
        {
            return i;
        }
    }

    return -1;
}

//! @brief Delete all outgoing arcs from a vertex in the directed graph.
//! @param graph - graph to delete from
//! @param index - index of the vertex
static void deleteOutgoingArcs(OLGraph* const graph, const int index)
{
    const ArcNode *curr = graph->xList[index].firstOut, *del = nullptr;
    while (curr)
    {
        del = curr;
        curr = curr->tailLink;
        ::delete del;
        del = nullptr;

        --graph->arcNum;
    }
    graph->xList[index].firstOut = nullptr;
}

//! @brief Delete all incoming arcs from a vertex in the directed graph.
//! @param graph - graph to delete from
//! @param index - index of the vertex
static void deleteIncomingArcs(OLGraph* const graph, const int index)
{
    for (int i = 0; i < graph->vexNum; ++i)
    {
        ArcNode *curr = graph->xList[i].firstOut, *prev = nullptr;
        while (curr)
        {
            if (curr->headVex != index)
            {
                prev = curr;
                curr = curr->tailLink;
                continue;
            }

            prev ? prev->tailLink = curr->tailLink : graph->xList[i].firstOut = curr->tailLink;
            ::delete curr;
            curr = nullptr;

            --graph->arcNum;
            curr = prev ? prev->tailLink : graph->xList[i].firstOut;
        }
    }
}

//! @brief Create the directed graph.
//! @param graph - graph to create
//! @param cmp - compare function to compare data
void create(OLGraph* const graph, const Compare cmp)
{
    graph->vexNum = 0;
    graph->arcNum = 0;
    graph->compare = cmp;
}

//! @brief Destroy the directed graph.
//! @param graph - graph to destroy
void destroy(OLGraph* const graph)
{
    if (!graph)
    {
        return;
    }

    const ArcNode *curr = nullptr, *del = nullptr;
    for (int i = 0; i < graph->vexNum; ++i)
    {
        curr = graph->xList[i].firstOut;
        while (curr)
        {
            del = curr;
            curr = curr->tailLink;
            ::delete del;
            del = nullptr;
        }
        graph->xList[i].firstIn = nullptr;
        graph->xList[i].firstOut = nullptr;
    }

    graph->vexNum = 0;
    graph->arcNum = 0;
}

//! @brief Insert a vertex into the directed graph.
//! @param graph - graph to insert into
//! @param vert - vert to insert
//! @return success or failure
bool insertVertex(OLGraph* const graph, const void* const vert)
{
    if (!graph || (graph->vexNum >= maxVertexNum) || (locateVertex(graph, vert) >= 0))
    {
        return false;
    }

    graph->xList[graph->vexNum].firstIn = nullptr;
    graph->xList[graph->vexNum].firstOut = nullptr;
    graph->xList[graph->vexNum].data = const_cast<void*>(vert);
    ++graph->vexNum;

    return true;
}

//! @brief Insert an arc between two vertices into the directed graph.
//! @param graph - graph to insert into
//! @param vert1 - vertex at the tail (source) of the arc
//! @param vert2 - vertex at the head (destination) of the arc
//! @return success or failure
bool insertArc(OLGraph* const graph, const void* const vert1, const void* const vert2)
{
    if (!graph)
    {
        return false;
    }

    const int index1 = locateVertex(graph, vert1), index2 = locateVertex(graph, vert2);
    if ((index1 < 0) || (index2 < 0))
    {
        return false;
    }

    const ArcNode* curr = graph->xList[index1].firstOut;
    while (curr)
    {
        if (curr->headVex == index2)
        {
            return false;
        }
        curr = curr->tailLink;
    }

    auto* const newNode = ::new (std::nothrow) ArcNode;
    newNode->headVex = index2;
    newNode->tailVex = index1;
    newNode->headLink = graph->xList[index2].firstIn;
    newNode->tailLink = graph->xList[index1].firstOut;
    graph->xList[index2].firstIn = graph->xList[index1].firstOut = newNode;
    ++graph->arcNum;

    return true;
}

//! @brief Delete a vertex from the directed graph.
//! @param graph - graph to delete from
//! @param vert - vert to delete
//! @return success or failure
bool deleteVertex(OLGraph* const graph, const void* const vert)
{
    if (!graph)
    {
        return false;
    }

    const int index = locateVertex(graph, vert);
    if (index < 0)
    {
        return false;
    }

    deleteOutgoingArcs(graph, index);
    deleteIncomingArcs(graph, index);

    for (int i = index; i < (graph->vexNum - 1); ++i)
    {
        graph->xList[i] = graph->xList[i + 1];
    }
    --graph->vexNum;

    for (int i = 0; i < graph->vexNum; ++i)
    {
        ArcNode* curr = graph->xList[i].firstOut;
        while (curr)
        {
            if (curr->tailVex > index)
            {
                --curr->tailVex;
            }
            if (curr->headVex > index)
            {
                --curr->headVex;
            }
            curr = curr->tailLink;
        }
    }

    return true;
}

//! @brief Delete an arc between two vertices from the directed graph.
//! @param graph - graph to delete from
//! @param vert1 - vertex at the tail (source) of the arc
//! @param vert2 - vertex at the head (destination) of the arc
//! @return success or failure
bool deleteArc(OLGraph* const graph, const void* const vert1, const void* const vert2)
{
    if (!graph)
    {
        return false;
    }

    const int index1 = locateVertex(graph, vert1), index2 = locateVertex(graph, vert2);
    if ((index1 < 0) || (index2 < 0))
    {
        return false;
    }

    ArcNode *curr = graph->xList[index1].firstOut, *prev = nullptr;
    while (curr)
    {
        if (curr->headVex != index2)
        {
            prev = curr;
            curr = curr->tailLink;
            continue;
        }

        prev ? prev->tailLink = curr->tailLink : graph->xList[index1].firstOut = curr->tailLink;
        ::delete curr;
        curr = nullptr;

        --graph->arcNum;
        return true;
    }

    return false;
}

void Traverse::dfs(const void* const vert, const Operation& op) const
{
    if (!graph)
    {
        return;
    }

    const int start = locateVertex(graph, vert);
    if (start < 0)
    {
        return;
    }

    bool visited[maxVertexNum] = {};
    dfsRecursive(start, visited, op);
}

void Traverse::bfs(const void* const vert, const Operation& op) const
{
    if (!graph)
    {
        return;
    }

    const int start = locateVertex(graph, vert);
    if (start < 0)
    {
        return;
    }

    bool visited[maxVertexNum] = {};
    visited[start] = true;
    op(graph->xList[start].data);

    int queue[maxVertexNum] = {}, front = 0, rear = 0;
    queue[rear++] = start;
    while (front < rear)
    {
        const int q = queue[front++];
        int neighbors[maxVertexNum], counter = 0;
        const ArcNode* curr = graph->xList[q].firstOut;
        while (curr)
        {
            neighbors[counter++] = curr->headVex;
            curr = curr->tailLink;
        }

        sortNeighbors(neighbors, counter);
        for (int i = 0; i < counter; ++i)
        {
            if (!visited[neighbors[i]])
            {
                visited[neighbors[i]] = true;
                op(graph->xList[neighbors[i]].data);
                queue[rear++] = neighbors[i];
            }
        }
    }
}

void Traverse::dfsRecursive(const int index, bool visited[], const Operation& op) const
{
    visited[index] = true;
    op(graph->xList[index].data);

    int neighbors[maxVertexNum], counter = 0;
    const ArcNode* curr = graph->xList[index].firstOut;
    while (curr)
    {
        neighbors[counter++] = curr->headVex;
        curr = curr->tailLink;
    }

    sortNeighbors(neighbors, counter);
    for (int i = 0; i < counter; ++i)
    {
        if (!visited[neighbors[i]])
        {
            dfsRecursive(neighbors[i], visited, op);
        }
    }
}

void Traverse::sortNeighbors(int neighbors[], const int size) const
{
    if (!graph->compare)
    {
        return;
    }

    for (int i = 1; i < size; ++i)
    {
        const int temp = neighbors[i];
        int j = i - 1;
        while ((j >= 0) && (graph->compare(graph->xList[neighbors[j]].data, graph->xList[temp].data) > 0))
        {
            neighbors[j + 1] = neighbors[j];
            --j;
        }
        neighbors[j + 1] = temp;
    }
}
} // namespace directed
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::graph
