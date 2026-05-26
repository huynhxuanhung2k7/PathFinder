#include "dijkstra.h"
#include <algorithm>        //reverse
#include <queue>            //priority queue
#include <functional>       //greater
#include <unordered_map>    //parent
#include <unordered_set>    //visited
#include <vector>

namespace {
    struct PQItem {
        Node* node = nullptr;
        int cost = 0;
        bool operator>(const PQItem& b) const {
            return this->cost > b.cost;
        }
    };
}

SearchResult Dijkstra::find_path(GridGraph& graph, Node* start, Node* goal) {
    SearchResult result;
    const auto t0 = std::chrono::steady_clock::now();
    
    if (!start || !goal || start->type == TileType::WALL || goal->type == TileType::WALL) {
        result.elapsed = std::chrono::steady_clock::now() - t0;
        return result;    
    }

    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> frontier;
    
    std::unordered_set<Node*> settled;
    std::unordered_map<Node*, Node*> parent;
    std::unordered_map<Node*, int> distance;

    distance[start] = 0;
    parent[start] = nullptr;
    frontier.push({start, 0});

    while (!frontier.empty()) {
        const PQItem top = frontier.top();
        frontier.pop();
        Node* current = top.node;

        
        if (settled.count(current) != 0) continue;
        settled.insert(current);
        result.explored_order.push_back(current);
        if (current == goal) break;
        
        for (Node* neighbor: graph.get_neighbors(current)) {
            if (settled.count(neighbor) != 0) continue;

            const int tentative = distance[current] + tile_cost(neighbor->type);

            if (distance.count(neighbor) == 0 || tentative < distance[neighbor]) {
                distance[neighbor] = tentative;
                parent[neighbor] = current;
                frontier.push({neighbor, tentative});
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

std::string Dijkstra::name() const {
    return "Dijkstra";
}