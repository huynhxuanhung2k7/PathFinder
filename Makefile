CXX        := clang++
CXXFLAGS   := -std=c++17 -Wall -Wextra -Iinclude -O2
LDFLAGS_NC := -lncurses

NCURSES_GRID := -DPATHFINDER_GRID_SIZE=25
RAYLIB_GRID  := -DPATHFINDER_GRID_SIZE=31

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  BREW_PREFIX   := $(shell brew --prefix 2>/dev/null)
  RAYLIB_CFLAGS := $(if $(BREW_PREFIX),-I$(BREW_PREFIX)/include)
  RAYLIB_LDFLAGS := $(if $(BREW_PREFIX),-L$(BREW_PREFIX)/lib)
  LDFLAGS_RL    := $(RAYLIB_LDFLAGS) -lraylib -lncurses -framework OpenGL -framework Cocoa -framework IOKit
else
  RAYLIB_CFLAGS :=
  LDFLAGS_RL    := -lraylib -lncurses
endif

SRCS    := $(wildcard src/*.cpp) main.cpp
OBJS_NC := $(SRCS:.cpp=.o)
OBJS_RL := $(SRCS:.cpp=.rl.o)

BIN_NC   := pathfinder
BIN_RL   := pathfinder_raylib

all: $(BIN_NC)

$(BIN_NC): $(OBJS_NC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS_NC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(NCURSES_GRID) -c $< -o $@

raylib: $(BIN_RL)

$(BIN_RL): $(OBJS_RL)
	$(CXX) $(CXXFLAGS) -DUSE_RAYLIB -o $@ $^ $(LDFLAGS_RL)

%.rl.o: %.cpp
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) -DUSE_RAYLIB $(RAYLIB_GRID) -c $< -o $@

clean:
	rm -f $(OBJS_NC) $(OBJS_RL) $(BIN_NC) $(BIN_RL)

