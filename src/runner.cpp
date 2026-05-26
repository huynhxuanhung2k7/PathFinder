#include "runner.h"
#include "dfs.h"
#include "bfs.h"
#include "dijkstra.h"
#include "astar.h"
#include <iostream>

AlgorithmRunner::AlgorithmRunner(std::unique_ptr<Renderer> renderer) :
    renderer_(std::move(renderer)) {
        setup_algorithms();
}

void AlgorithmRunner::setup_algorithms() {
    algorithms_.push_back(std::make_unique<BFS>());
    algorithms_.push_back(std::make_unique<DFS>());
    algorithms_.push_back(std::make_unique<Dijkstra>());
    algorithms_.push_back(std::make_unique<AStar>());
}

void AlgorithmRunner::run_all_algorithms() {
    frames_.clear();

    for (const auto& algo: algorithms_) {
        SearchResult r = algo->find_path(graph_, graph_.start(), graph_.goal());
        frames_.push_back({ algo->name(), std::move(r), 0, false });
    }
}

void AlgorithmRunner::run() {
    for (const auto& f: frames_) {
        std::cout   << f.label
                    << " | visited=" << f.result.explored_size
                    << " | cost="    << f.result.cost
                    << " | time="    << f.result.elapsed.count() << "ms\n";
    }
}