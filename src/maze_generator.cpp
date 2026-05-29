#include "maze_generator.h"

#include <vector>
#include <cstdlib>
#include <numeric>
#include <utility>
#include <algorithm>

namespace {
    struct DisjointSet {
        std::vector<int> parent;
        std::vector<int> rank;

        explicit DisjointSet(int n) : parent(n), rank(n, 0) {
            std::iota(parent.begin(), parent.end(), 0); //parent[i] = i
        }

        int find(int x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }

        void unite(int a, int b) {
            int ra = find(a), rb = find(b); //find root
            if (ra == rb) return;
            if (rank[ra] < rank[rb]) std::swap(ra, rb); //make sure ra is always the root
            parent[rb] = ra;
            if (rank[ra] == rank[rb]) rank[ra]++;
        }
    };
}

MazeGenerator::MazeGenerator(unsigned int seed) : seed_(seed), rng_(seed) {}
unsigned int MazeGenerator::seed() const { return seed_; }

void MazeGenerator::generate(GridGraph& grid) {
    grid.fill_walls();

    constexpr int W = GridGraph::WIDTH;
    constexpr int H = GridGraph::HEIGHT;
    const int cells_per_row = (W + 1) / 2; //the maze start with Cell - Wall - Cell - ...
    const int cells_per_col = (H + 1) / 2;

    for (int y = 0; y < H; y += 2) 
        for (int x = 0; x < W; x += 2)
            grid.set_tile(x, y, TileType::EMPTY);

    auto cell_id = [](int x, int y) {
        return (y / 2) * cells_per_row + (x / 2);
    };

    struct Edge { int a; int b; int wx; int wy; };
    std::vector<Edge> edges;
    for (int y = 0; y < H; y+=2)
        for (int x = 0; x < W; x+=2) {
            if (x + 2 < W) edges.push_back({cell_id(x, y), cell_id(x+2, y), x + 1, y});
            if (y + 2 < H) edges.push_back({cell_id(x, y), cell_id(x, y+2), x, y + 1});
        }
    
    std::shuffle(edges.begin(), edges.end(), rng_);

    DisjointSet ds(cells_per_row * cells_per_col);
    for (const auto& edge: edges) {
        if (ds.find(edge.a) != ds.find(edge.b)) {
            ds.unite(edge.a, edge.b);
            grid.set_tile(edge.wx, edge.wy, TileType::EMPTY);
        }
    }

    std::uniform_int_distribution<int> braid(0, 99);
    for (const auto& edge: edges) 
        if (!grid.passable(edge.wx, edge.wy) && braid(rng_) < 15)
            grid.set_tile(edge.wx, edge.wy, TileType::EMPTY);
}

void MazeGenerator::scatter_terrain(GridGraph& grid) {
    std::uniform_int_distribution<int> roll(0, 99);
    for (int y = 0; y < GridGraph::HEIGHT; y++) 
        for (int x = 0; x < GridGraph::WIDTH; x++) {
            if (!grid.passable(x, y)) continue;
            const int r = roll(rng_);
            if      (r < 55) continue;
            else if (r < 78) grid.set_tile(x, y, TileType::GRASS);
            else if (r < 93) grid.set_tile(x, y, TileType::MUD);
            else             grid.set_tile(x, y, TileType::WATER);
        }
}

void MazeGenerator::place_endpoints(GridGraph& grid, int min_distance) {
    const int cells_per_row = (GridGraph::WIDTH + 1) / 2; //the maze start with Cell - Wall - Cell - ...
    const int cells_per_col = (GridGraph::HEIGHT + 1) / 2;

    std::uniform_int_distribution<int> pick_row(0, cells_per_row - 1);
    std::uniform_int_distribution<int> pick_col(0, cells_per_col - 1);

    int start_x, start_y, goal_x, goal_y;
    for (int tries = 0; ;tries++) {
        start_x = pick_col(rng_) * 2; start_y = pick_row(rng_) * 2;
        goal_x =  pick_col(rng_) * 2;  goal_y =  pick_row(rng_) * 2;
        const int dist = std::abs(start_x - goal_x) + std::abs(start_y - goal_y);
        if (dist >= min_distance) break;
        if (tries > 1000) break; 
    }
    grid.set_start(start_x, start_y);
    grid.set_goal(goal_x, goal_y);
}