#include "runner.h"
#include "ncurses_renderer.h"
#include "graph.h"
#include "maze_generator.h"

#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <queue>
#include <array>
#include <cstdlib>
#include <utility>


int main() {
    try {
        auto renderer = std::make_unique<NcursesRenderer>();
        AlgorithmRunner runner(std::move(renderer));
        return runner.run();
    } catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Fatal: unknown error\n";
        return 1;
    }
}