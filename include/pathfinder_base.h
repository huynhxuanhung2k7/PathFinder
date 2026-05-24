#pragma once

#include "graph.h"
#include <string>
#include <chrono>
#include <vector>

struct SearchResult {
    std::vector<Node*> path;            //start -> goal
    std::vector<Node*> explored_order;  //explored order for drawing
    int cost = 0;                       // via path_cost
    int explored_size = 0;               // via explored_order.size()

    std::chrono::duration<double, std::milli> elapsed{0.0};
};

//parent class for algorithm
class PathFinderBase {
    public:
    virtual ~PathFinderBase() = default;
    virtual SearchResult find_path(GridGraph& graph, Node* start, Node* goal) = 0;
    virtual std::string name() const = 0;

    protected:
    PathFinderBase() = default;
};