#pragma once
#include "renderer.h"
#include "graph.h"
#include "pathfinder_base.h"
#include "maze_generator.h"

#include <vector>
#include <memory>

class AlgorithmRunner {
    public:
    explicit AlgorithmRunner(std::unique_ptr<Renderer> renderer);
    int run();

    private: //data
    GridGraph graph_;
    std::vector<std::unique_ptr<PathFinderBase>> algorithms_;
    std::unique_ptr<Renderer> renderer_;
    std::vector<AlgorithmFrame> frames_;
    MazeGenerator maze_;
    //helper
    void generate_maze();
    void setup_algorithms();
    void run_all_algorithms();  //find path
    void advance_animation();
};

