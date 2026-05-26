#include "graph.h"
#include <limits>

int tile_cost(TileType t) {
	switch (t) {
	case TileType::EMPTY:
		return 1;
	case TileType::GRASS:
		return 3;
	case TileType::MUD:
		return 5;
	case TileType::WATER:
		return 8;
	case TileType::WALL:
		return std::numeric_limits<int>::max() / 4;  // divide 4 for safety 
	default:
		return std::numeric_limits<int>::max() / 4;
	}
}

Node::Node(int r, int c, TileType t)
    : row(r), col(c), cost(tile_cost(t)), type(t) {}


GridGraph::GridGraph() {
	clear();
}
void GridGraph::clear() {
	for (int r = 0; r < HEIGHT; ++r) {
		for (int c = 0; c < WIDTH; ++c) {
			tiles_[r][c] = Node{r, c, TileType::EMPTY};
		}
	}
}

void GridGraph::clear_walls() {
	for (auto& row: tiles_) 
		for (auto& node: row) 
			if (node.type == TileType::WALL) {
				node.type = TileType::EMPTY;
				node.cost = tile_cost(TileType::EMPTY);
			}
}

bool GridGraph::in_bounds(int x, int y) const {
	return (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT);
}

bool GridGraph::passable(int x, int y) const {
	return (in_bounds(x, y) && (tiles_[y][x].type != TileType::WALL));
}

Node* GridGraph::get_node(int x, int y) {
	if (in_bounds(x, y)) return &tiles_[y][x];
	return nullptr;
}

const Node* GridGraph::get_node(int x, int y) const {
	if (in_bounds(x, y)) return &tiles_[y][x];
	return nullptr;
}

void GridGraph::set_tile(int x, int y, TileType type) {
	if (!in_bounds(x, y)) return;
	tiles_[y][x].type = type;
	tiles_[y][x].cost = tile_cost(type); 
}

std::vector<Node*> GridGraph::get_neighbors(int x, int y) {
	std::vector<Node*> out;
	out.reserve(4);

	constexpr std::array<std::pair<int, int>, 4> dirs = {{
		{0, -1},   // up
        {1,  0},   // right
        {0,  1},   // down
        {-1, 0},   // left
	}};

	for (const auto& [dx, dy] : dirs) 
		if (passable(x + dx, y + dy)) 
			out.push_back(&tiles_[y+dy][x+dx]);
	return out;
}

std::vector<Node*> GridGraph::get_neighbors(Node* node) {
	// node->col is the x-coordinate, node->row is the y-coordinate.
	// The (int x, int y) overload expects them in that order — swapping
	// here means BFS/DFS/Dijkstra/A* all explore a transposed grid.
	return get_neighbors(node->col, node->row);
}

Node*       GridGraph::start()       { return &tiles_[start_y_][start_x_]; }
const Node* GridGraph::start() const { return &tiles_[start_y_][start_x_]; }
Node*		GridGraph::goal() 		 { return &tiles_[goal_y_][goal_x_]; }
const Node* GridGraph::goal()  const { return &tiles_[goal_y_][goal_x_]; }

void GridGraph::set_start(int x, int y) {
	if (!in_bounds(x, y)) return;
	start_x_ = x;
	start_y_ = y;
	set_tile(x, y, TileType::EMPTY);
}

void GridGraph::set_goal(int x, int y) {
	goal_x_ = x;
	goal_y_ = y;
	set_tile(x, y, TileType::EMPTY);
}

int path_cost(const std::vector<Node*>& path) {
	if (path.size() < 2) return 0;

	int total = 0;
	for (std::size_t i= 1; i < path.size(); ++i) 
		total += tile_cost(path[i]->type);
	
	return total;
}


