#ifdef USE_RAYLIB
    #include "raylib_renderer.h"
#else
    #include "ncurses_renderer.h"
#endif

#include "runner.h"

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
#ifdef USE_RAYLIB
            auto renderer = std::make_unique<RaylibRenderer>();
#else
            auto renderer = std::make_unique<NcursesRenderer>();
#endif

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