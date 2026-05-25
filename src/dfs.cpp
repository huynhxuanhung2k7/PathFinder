#include "dfs.h"

#include <string>
#include <stack>
#include <unordered_map>
#include <unordered_set> 
#include <algorithm>

SearchResult DFS::find_path(GridGraph& graph, Node* start, Node* goal) {
    SearchResult result;
    const auto t0 = std::chrono::steady_clock::now(); 

    if (!start || !goal || start->type == TileType::WALL || goal->type == TileType::WALL) {
        result.elapsed = std::chrono::steady_clock::now() - t0;
        return result;    
    }

    std::stack<Node*> frontier;
    std::unordered_set<Node*> visited;
    std::unordered_map<Node*, Node*> parent;

    frontier.push(start);
    parent[start] = nullptr;

    while (!frontier.empty()) {
        Node* current = frontier.top();
        if (current == goal) break;
        frontier.pop();

        if (visited.count(current) > 0) continue;
        visited.insert(current);
        result.explored_order.push_back(current);

        for (Node* neighbor: graph.get_neighbors(current)) {
            if (visited.count(neighbor) == 0) {
                frontier.push(neighbor);
                parent[neighbor] = current;
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

std::string DFS::name() const {
    return "DFS";
}