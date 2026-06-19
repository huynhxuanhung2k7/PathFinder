# PathFinder

A terminal and GUI visualiser for BFS, DFS, Dijkstra, and A* running side-by-side on a weighted 2D grid. Built in C++17 for FIT1045 at Monash University.

Two rendering backends:
- **ncurses** — runs in any terminal, no GUI needed
- **Raylib** — windowed GUI with colours and smooth animation

---

## Features

- Four algorithms animating simultaneously: BFS, DFS, Dijkstra, A*
- Weighted terrain tiles — EMPTY (1), GRASS (3), MUD (5), WATER (8), WALL (impassable)
- Seeded procedural maze generation (kruskal + terrain scatter via `std::mt19937`)
- Per-algorithm stats: visited count, path cost, elapsed time
- Interactive controls: paint walls, regenerate maze, clear, re-run
- Maze seeds printed on exit for reproducibility

---

## Requirements

You need `clang++` or `g++` with C++17 support, plus the rendering libraries below.

### ncurses build (terminal UI)

| Platform | Install |
|---|---|
| macOS | Usually pre-installed. If not: `brew install ncurses` |
| Ubuntu / Debian | `sudo apt install libncurses-dev` |
| Windows | Use WSL (see below) |

### Raylib build (GUI window)

| Platform | Install |
|---|---|
| macOS | `brew install raylib` |
| Ubuntu / Debian | `sudo apt install libraylib-dev` or build from source (see below) |
| Windows | Use WSL (see below) |

**macOS — install everything at once:**
```bash
xcode-select --install
brew install raylib ncurses
```

**Ubuntu / Debian:**
```bash
sudo apt update
sudo apt install build-essential clang libncurses-dev
```

**Raylib from source on Linux** (if `libraylib-dev` is not in your package manager):
```bash
sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

---

## Build & Run

Clone the repo:
```bash
git clone https://github.com/huynhxuanhung2k7/PathFinder.git
cd PathFinder
```

### macOS (Makefile works as-is)

```bash
# Terminal UI (ncurses)
make
./pathfinder

# GUI window (Raylib)
make raylib
./pathfinder_raylib

# Clean build artifacts
make clean
```

### Linux — terminal UI

The ncurses target works without changes:
```bash
make
./pathfinder
```

### Linux — Raylib GUI

The Makefile has macOS-specific flags (`-framework OpenGL -framework Cocoa -framework IOKit`) that don't exist on Linux. Compile manually instead:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -O2 -DUSE_RAYLIB -DPATHFINDER_GRID_SIZE=31 \
    src/graph.cpp src/bfs.cpp src/dfs.cpp src/dijkstra.cpp src/astar.cpp \
    src/maze_generator.cpp src/raylib_renderer.cpp src/runner.cpp main.cpp \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -o pathfinder_raylib
./pathfinder_raylib
```

> You can also edit the `Makefile` directly — replace the three `-framework` lines with `-lGL -lm -lpthread -ldl -lrt -lX11` to make `make raylib` work on Linux natively.

### Windows (WSL recommended)

1. Install [WSL 2](https://learn.microsoft.com/en-us/windows/wsl/install) with Ubuntu.
2. Open Ubuntu and follow the Linux steps above.
3. For the Raylib GUI inside WSL, install an X server (e.g. [VcXsrv](https://sourceforge.net/projects/vcxsrv/)) and run `export DISPLAY=:0` before launching.

---

## Controls

| Key | Action |
|---|---|
| `WASD` / Arrow keys | Move cursor |
| `SPACE` | Toggle wall at cursor |
| `R` | Re-run all algorithms |
| `G` | Generate a new seeded maze |
| `C` | Clear grid |
| `Q` / `ESC` | Quit |
| `F` | Toggle fullscreen *(Raylib only)* |

---

## Tile costs

| Tile | Traversal cost |
|---|---|
| Empty | 1 |
| Grass | 3 |
| Mud | 5 |
| Water | 8 |
| Wall | impassable |

Dijkstra and A* route around expensive terrain; BFS and DFS treat all passable tiles equally (unweighted).

---

## Architecture

Three-layer separation — data, algorithm, and presentation are fully decoupled.

```
main.cpp
  └── AlgorithmRunner
        ├── GridGraph              single source of truth for the world
        ├── BFS      }
        ├── DFS      }             PathFinderBase concrete implementations
        ├── Dijkstra }
        └── AStar    }
        └── Renderer               swappable backend
              ├── NcursesRenderer
              └── RaylibRenderer
```

`AlgorithmRunner` is the only class that touches all three layers. Each algorithm returns a `SearchResult { path, explored_order, cost }` — the renderer animates each independently at ~28 FPS.

---

## Project structure

```
PathFinder/
├── main.cpp                  Entry point — picks renderer via USE_RAYLIB flag
├── Makefile
├── include/
│   ├── graph.h               TileType, Node, GridGraph
│   ├── pathfinder_base.h     PathFinderBase abstract + SearchResult
│   ├── bfs.h / dfs.h
│   ├── dijkstra.h / astar.h
│   ├── maze_generator.h      Seeded maze gen (recursive backtracker)
│   ├── renderer.h            Renderer abstract + AlgorithmFrame
│   ├── ncurses_renderer.h    Terminal backend
│   ├── raylib_renderer.h     GUI backend
│   └── runner.h              AlgorithmRunner orchestrator
└── src/                      Corresponding .cpp files
```

---

## Algorithms

| Algorithm | Weighted? | Optimal? | Notes |
|---|---|---|---|
| BFS | No | Yes (unweighted) | Explores by hop count |
| DFS | No | No | Fast but non-optimal paths |
| Dijkstra | Yes | Yes | Min-heap on cumulative cost |
| A* | Yes | Yes | Dijkstra + Manhattan heuristic |

---

*FIT1045 custom project · Monash University · S1 2026*  
*Author: Huỳnh Xuân Hùng (Harry)*