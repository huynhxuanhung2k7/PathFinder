#include "astar.h"
#include <algorithm>        //reverse
#include <cstdlib>          //std::abs
#include <queue>            //priority queue
#include <functional>       //greater
#include <unordered_map>    //parent, distance (g-score)
#include <unordered_set>    //settled (closed set)
#include <vector>

namespace {
    struct PQItem {
        Node* node = nullptr;
        int cost = 0;
        bool operator>(const PQItem& b) const {
            return this->cost > b.cost;
        }
    };

    int heuristic(const Node* a, const Node* b) {
        return std::abs(b->row - a->row) + std::abs(b->col - a->col);
    }
}

SearchResult AStar::find_path(GridGraph& graph, Node* start, Node* goal) {
    SearchResult result;
    const auto t0 = std::chrono::steady_clock::now();

    if (!start || !goal || start->type == TileType::WALL || goal->type == TileType::WALL) {
        result.elapsed = std::chrono::steady_clock::now() - t0;
        return result;    
    }

    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> frontier;
    
    std::unordered_map<Node*, Node*> parent;
    std::unordered_map<Node*, int> distance;
    std::unordered_set<Node*> settled;
    
    parent[start] = nullptr;
    distance[start] = 0;
    frontier.push({start, heuristic(start, goal)});

    while (!frontier.empty()) {
        const PQItem top = frontier.top();
        frontier.pop();
        Node* current = top.node;

        if (settled.count(current) !=0 ) continue;
        settled.insert(current);
        result.explored_order.push_back(current);
        if (current == goal) break;

        for (Node* neighbor: graph.get_neighbors(current)) {
            if (settled.count(neighbor) != 0) continue;

            const int tentative =  distance[current] + tile_cost(neighbor->type);

            if (distance.count(neighbor) == 0 || tentative < distance[neighbor]) {
                distance[neighbor] = tentative;
                parent[neighbor] = current;
                frontier.push({neighbor, tentative + heuristic(neighbor, goal)});
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

std::string AStar::name() const {
    return "AStar";
}