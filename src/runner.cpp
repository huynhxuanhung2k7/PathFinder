#include "runner.h"
#include "dfs.h"
#include "bfs.h"
#include "dijkstra.h"
#include "astar.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace {
    constexpr int TICK_MS = 35;
}

AlgorithmRunner::AlgorithmRunner(std::unique_ptr<Renderer> renderer) 
    :   renderer_(std::move(renderer)), maze_(std::random_device{}()) {
    setup_algorithms();
}

void AlgorithmRunner::setup_algorithms() {
    algorithms_.push_back(std::make_unique<BFS>());
    algorithms_.push_back(std::make_unique<DFS>());
    algorithms_.push_back(std::make_unique<Dijkstra>());
    algorithms_.push_back(std::make_unique<AStar>());
}

void AlgorithmRunner::generate_maze() {
    maze_.generate(graph_);
    maze_.scatter_terrain(graph_);
    maze_.place_endpoints(graph_, 20);
    run_all_algorithms();
}

void AlgorithmRunner::run_all_algorithms() {
    constexpr int TIMING_RUNS = 300;
    frames_.clear();
    frames_.reserve(algorithms_.size());
    for (const auto& algo: algorithms_) {
        SearchResult r = algo->find_path(graph_, graph_.start(), graph_.goal());
        
        auto best = std::chrono::nanoseconds::max();
        for (int i = 0; i < TIMING_RUNS; i++) {
            auto t0 = std::chrono::steady_clock::now();
            const SearchResult t = algo->find_path(graph_, graph_.start(), graph_.goal());
            auto t1 = std::chrono::steady_clock::now();
            best = std::min(best, std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0));
        }
        r.elapsed = best;
        
        frames_.push_back({ algo->name(), std::move(r), 0, false });
    }
}

int AlgorithmRunner::run() {
    renderer_->init();
    generate_maze();

    while (!renderer_->should_close()) {
        const InputAction action = renderer_->take_input();
        switch (action) {
            case InputAction::GENERATE:
                generate_maze();
                break;
            case InputAction::RERUN:
                run_all_algorithms();
                break;
            case InputAction::CLEAR:
                graph_.clear();
                run_all_algorithms();
                break;
            case InputAction::FULLSCREEN: //raylib later
            case InputAction::NONE:
                break;
            case InputAction::QUIT: 
                renderer_->shutdown();
                std::cout << "maze seed: " << maze_.seed() << "\n";
                return 0;
        }

        advance_animation();
        renderer_->render(frames_, graph_);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
    }

    renderer_->shutdown();
    return 0;
}

void AlgorithmRunner::advance_animation() {
    for (AlgorithmFrame& frame: frames_) {
        if (frame.finished) continue;
        
        const auto& explored = frame.result.explored_order;
        const auto& path = frame.result.path;

        const std::size_t total = explored.size() + path.size();
        if (frame.animation_step >= static_cast<int>(total)) {
            frame.finished = true;
            continue;
        }

        const std::size_t step = static_cast<std::size_t>(frame.animation_step);
        Node* current = (step < explored.size()) 
                      ? explored[step]
                      : path[step - explored.size()];
        
        if (!current) {
            frame.animation_step++;
            frame.frames_held = 0;
            continue;
        }

        const int hold_for = (step < explored.size())
                           ? tile_cost(current->type)
                           : 1;
        if (++frame.frames_held >= hold_for) {
            ++frame.animation_step;
            frame.frames_held = 0;
        }
    }
}