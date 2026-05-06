#include "graph.h"
#include <algorithm>

int tile_cost(TileType t) {
    switch (t)
    {
    case TileType::GRASS : return 3;
    case TileType::MUD: return 5;
    case TileType::WATER: return 8;    
    default: return 1; //EMPTY case
    }
}

Node::Node(int r, int c, TileType t) :
    row(r), col(c), type(t), cost(tile_cost(t)) {}

