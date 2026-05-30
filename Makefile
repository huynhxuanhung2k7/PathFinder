CXX      := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -O2

# Every source except the two renderers — each build adds exactly one renderer.
COMMON := $(filter-out src/ncurses_renderer.cpp src/raylib_renderer.cpp, $(wildcard src/*.cpp)) main.cpp

#terminal UI:   make
pathfinder: $(COMMON) src/ncurses_renderer.cpp
	$(CXX) $(CXXFLAGS) -DPATHFINDER_GRID_SIZE=25 $^ -lncurses -o $@

#Raylib window:   make raylib 
raylib: $(COMMON) src/raylib_renderer.cpp
	$(CXX) $(CXXFLAGS) -DUSE_RAYLIB -DPATHFINDER_GRID_SIZE=31 \
	       -I$(shell brew --prefix)/include $^ \
	       -L$(shell brew --prefix)/lib -lraylib -lncurses \
	       -framework OpenGL -framework Cocoa -framework IOKit \
	       -o pathfinder_raylib

clean:
	rm -f pathfinder pathfinder_raylib

.PHONY: raylib clean
