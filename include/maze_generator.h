#pragma once    

#include <random>
#include "graph.h"
class MazeGenerator{
    public:
    explicit MazeGenerator(unsigned int seed);
    void generate(GridGraph& grid);
    void place_endpoints(GridGraph& grid, int min_distance);
    void scatter_terrain(GridGraph& grid);
    unsigned int seed() const;
    
    private:
    unsigned int seed_;
    std::mt19937 rng_;
};
