#pragma once 

#include "pathfinder_base.h"

class DFS: public PathFinderBase {
    public:
    SearchResult find_path(GridGraph& graph, Node* start, Node* goal) override;
    std::string name() const override;
};