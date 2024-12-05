#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

struct Node
{
    int id;
    double latitude;
    double longitude;
};

struct Edge
{
    int id;
    int node1;
    int node2;
    double distance;
};

class Graph
{
private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    void loadNodes(const std::string &filename);
    void loadEdges(const std::string& filename);
    void default_graph();

public:
    Graph();

    Graph(const std::string &nodesFilename, const std::string &edgesFilename);

    std::vector<Node> getNodes() const;

    std::vector<Edge> getEdges() const;

    Edge getEdge(int index) const;
    Node getNode(int index) const;
};

#endif // GRAPH_H
