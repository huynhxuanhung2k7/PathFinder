#include "runner.h"
#include "ncurses_renderer.h"

#include <iostream>
#include <exception>
#include <memory>


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