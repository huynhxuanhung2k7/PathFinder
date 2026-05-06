CXX      = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRC      = $(wildcard src/*.cpp)
OBJ      = $(SRC:.cpp=.o)
TARGET   = pathfinder

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean


# # Makefile — PathfinderXP

# CXX      := clang++
# CXXFLAGS := -std=c++17 -Wall -Iinclude


# SRCS := src/graph.cpp \
#         src/bfs.cpp \
#         src/dfs.cpp \
#         src/dijkstra.cpp \
#         src/astar.cpp \
#         src/maze_generator.cpp \
#         src/runner.cpp


# # NCURSES_SRCS := $(SRCS) src/ncurses_renderer.cpp main.cpp
# # NCURSES_OUT  := pathfinder

# # ncurses: $(NCURSES_SRCS)
# # 	$(CXX) $(CXXFLAGS) -o $(NCURSES_OUT) $(NCURSES_SRCS) -lncurses

# # RAYLIB_SRCS := $(SRCS) src/raylib_renderer.cpp main.cpp
# # RAYLIB_OUT  := pathfinder_gl

# # raylib: $(RAYLIB_SRCS)
# # 	$(CXX) $(CXXFLAGS) -o $(RAYLIB_OUT) $(RAYLIB_SRCS) -lraylib -lm


# clean:
# 	rm -f $(NCURSES_OUT) $(RAYLIB_OUT)

# # .PHONY: ncurses raylib clean