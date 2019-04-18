# Insight - Graphs library

This library implements a [graph](https://en.wikipedia.org/wiki/Graph_(discrete_mathematics)) class, and a few graph theory algorithms. This library supports vertices of any types: `int`, `std::string`, pointers... Edges can also have a unique label of any type.


# Overview

## Graphs

### BasicUndirectedGraph

[BasicUndirectedGraph](include/BasicUndirectedGraph.hpp) is the low level implementation of a graph, which supports only `int` as a vertices.

### UndirectedGraph

[UndirectedGraph](include/UndirectedGraph.hpp) wraps a [BasicUndirectedGraph](include/BasicUndirectedGraph.hpp) and enables adding a label to its vertices and edges.

## Algorithms

Algorithms may come in two variants implemented by different classes. The "Basic" one works on [BasicUndirectedGraph](include/BasicUndirectedGraph.hpp) objects, while the other works on [UndirectedGraph](include/UndirectedGraph.hpp) objects.

### (Basic) MinimumSpanningTree

Attempts to compute an unweighted minimum spanning tree on a given graph, with the given node as its root. It is possible to:

- check that the graph is connected (the tree cover every vertices of the graph)
- check that the graph is acyclic (the tree cover is unique)
- read the tree
- apply an algorithm on each vertex/edge pair of the tree in a depth-first order
