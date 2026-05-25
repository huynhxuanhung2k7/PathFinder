# PathFinder

---

## Build

**Dependencies:** `clang++` (C++17), `ncurses`, `raylib`

```bash
# Terminal backend (ncurses)
make

# GUI backend (Raylib)
make raylib
```

Tested on macOS with `clang++ -std=c++17 -Wall -Wextra`.

---
# Architecture

Three-layer design — data, algorithm, and presentation are fully decoupled.

```
main.cpp
  └── AlgorithmRunner
        ├── GridGraph          ← single source of truth for the world
        ├── BFS                ← PathfinderBase concrete implementations
        ├── DFS
        ├── Dijkstra
        ├── AStar
        └── Renderer           ← swappable backend
              ├── NcursesRenderer
              └── RaylibRenderer
```

The `AlgorithmRunner` is the only class that touches all three layers.
Each algorithm returns a `SearchResult { path, explored_order, cost }` —
the renderer animates them independently at ~28 FPS.

---

## Grid & tile costs

21 × 21 grid. Start: `(0, 0)` · Goal: `(20, 20)`.

| Tile   | Movement cost |
|--------|:---:|
| Empty  | 1   |
| Grass  | 3   |
| Mud    | 5   |
| Water  | 8   |
| Wall   | impassable |

---
