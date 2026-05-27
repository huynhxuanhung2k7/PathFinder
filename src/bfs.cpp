#include "bfs.h"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

SearchResult BFS::find_path(GridGraph& graph, Node* start, Node* goal) {
    SearchResult result;
    const auto t0 = std::chrono::steady_clock::now();   //start to count time
    
    if (!start || !goal || start->type == TileType::WALL || goal->type == TileType::WALL) {
        result.elapsed = std::chrono::steady_clock::now() - t0;
        return result;    
    }

    std::queue<Node*> frontier;
    std::unordered_set<Node*> visited;
    std::unordered_map<Node*, Node*> parent;

    frontier.push(start);
    visited.insert(start);
    parent[start] = nullptr;

    while (!frontier.empty()) {
        Node* current = frontier.front();
        frontier.pop();
        result.explored_order.push_back(current);   //render the animations for this sequence
        
        if (current == goal) break;
        
        for (Node* neighbor: graph.get_neighbors(current)) {
            if (visited.count(neighbor) == 0) {
                visited.insert(neighbor);
                parent[neighbor] = current;
                frontier.push(neighbor);
            }
        }  
    }

    result.elapsed = std::chrono::steady_clock::now() - t0;
    result.explored_size = static_cast<int>(result.explored_order.size());

    if (parent.find(goal) != parent.end()) {
        for (Node* node = goal; node != nullptr; node = parent[node]) {
            result.path.push_back(node);
        }
        
        std::reverse(result.path.begin(), result.path.end());

        for (Node* node: result.path) {
            if (node != start) {
                result.cost += tile_cost(node->type);
            }
        }
    }
   
    return result;
}

std::string BFS::name() const {
    return "BFS";
}