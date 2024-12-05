#include "graph.h"

#include <fstream>
#include <iostream>

void Graph::loadNodes(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error abriendo el archivo de nodos" << std::endl;
        return;
    }

    Node node;
    while (file >> node.id >> node.latitude >> node.longitude)
    {
        this->nodes.push_back(node);
    }
};

void Graph::loadEdges(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error abriendo el archivo de aristas" << std::endl;
        return;
    }

    Edge edge;
    while (file >> edge.id >> edge.node1 >> edge.node2 >> edge.distance) {
        edges.push_back(edge);
    }
};

void Graph::default_graph() {
    nodes.clear();
    edges.clear();

    // Crear un grafo pequeño por defecto
    nodes = {
        {0, 40.7128, -74.0060},   // New York
        {1, 34.0522, -118.2437},  // Los Angeles
        {2, 41.8781, -87.6298},   // Chicago
        {3, 29.7604, -95.3698},    // Houston
        {4, 37.7749, -122.4194}   // San Francisco
    };

    edges = {
        {0, 0, 1, 4},   // New York to Los Angeles
        {1, 0, 2, 7},   // New York to Chicago
        {2, 1, 2, 24},   // Los Angeles to Chicago
        {3, 1, 3, 8},   // Los Angeles to Houston
        {4, 2, 3, 16},    // Chicago to Houston
        {5, 1, 4, 6},   // Los Angeles to San Francisco
        {6, 3, 4, 21}   // Houston to San Francisco
    };
}

Graph::Graph() {
    this->default_graph();
};

Graph::Graph(const std::string &nodesFilename, const std::string &edgesFilename) {
    this->loadNodes(nodesFilename);
    this->loadEdges(edgesFilename);
}

std::vector<Node> Graph::getNodes() const {
    return this->nodes;
};

std::vector<Edge> Graph::getEdges() const {
    return this->edges;
};

Edge Graph::getEdge(int index) const {
    return this->edges[index];
};

Node Graph::getNode(int index) const {
    return this->nodes[index];
};

