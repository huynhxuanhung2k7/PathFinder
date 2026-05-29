#pragma once

#include <array>    //for storing pure graph
#include <vector>

enum class TileType {
    EMPTY,  //cost 1
    GRASS,  //cost 3
    MUD,    //cost 5
    WATER,  //cost 8
    WALL,   //max int 
};

int tile_cost(TileType t);

struct Node {
    int row = 0;
    int col = 0;
    int cost = 1;
    TileType type = TileType::EMPTY;
    
    Node() = default;
    Node(int r, int c, TileType t = TileType::EMPTY);
}; 

#ifndef PATHFINDER_GRID_SIZE 
#define PATHFINDER_GRID_SIZE 25
#endif

static_assert(PATHFINDER_GRID_SIZE % 2 == 1, "Grid size must be odd for maze generation ");


class GridGraph {
    public:
    static constexpr int WIDTH = PATHFINDER_GRID_SIZE;
    static constexpr int HEIGHT = PATHFINDER_GRID_SIZE;

    private:
    std::array<std::array<Node, WIDTH>, HEIGHT> tiles_; 
    int start_x_ = 0;
    int start_y_ = 0;
    int goal_x_ = WIDTH - 1;
    int goal_y_ = HEIGHT - 1;
    
    public:
    GridGraph();        
    void clear();       // reset all tiles to EMPTY
    void clear_walls(); //remove walls
    void fill_walls();
    void set_start(int x, int y);
    void set_goal(int x, int y);
    

    
    bool in_bounds(int x, int y) const; //return true if (x, y) is within grid bounds
    bool passable(int x, int y) const;  //return if (x, y) in_bounds and not a wall

    //Return a pointer to the Node at (x, y), or nullptr if it is not bounds
    Node*       get_node(int x, int y);
    const Node* get_node(int x, int y) const;

    //Change a tile's type and update its cost field to match.
    void set_tile(int x, int y, TileType type);

    //Returns up to 4 passable neighbors 
    // Primary navigation for 4 algorithms
    std::vector<Node*> get_neighbors(int x, int y);
    std::vector<Node*> get_neighbors(Node* node);

    Node*       start();
    Node*       goal();
    const Node* start() const;
    const Node* goal()  const;
};

int path_cost(const std::vector<Node*>& path);