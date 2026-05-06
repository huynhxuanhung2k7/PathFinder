#pragma once
#include <vector>

enum class TileType {
    EMPTY,  //cost 1
    GRASS,  //cost 3
    MUD,    //cost 5
    WATER,  //cost 8
};

int tile_cost(TileType t);

struct Node {
    int row, col;
    TileType type;
    int cost;
    
    Node(int r, int c, TileType t = TileType::EMPTY);
};

struct Edge {
    int to;
    int weight;
};



