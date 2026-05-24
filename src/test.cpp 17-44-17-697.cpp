#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

std::vector<int> dfs(const std::vector<std::vector<int>>& graph, int start, int goal) 
{
    std::unordered_set<int> visited;
    std::unordered_map<int, int> came_from;
    std::vector<int> explored_order;
    
    std::stack<int> frontier;
    frontier.push(start);
    came_from[start] = start;

    while (!frontier.empty())
    {    
        int current = frontier.top();
        frontier.pop();

        if (visited.count(current)) continue;

        visited.insert(current);
        explored_order.push_back(current);

        if (current == goal) break;

        const auto& neighbors = graph[current];
        for (int i = static_cast<int>(neighbors.size()) - 1; i >= 0; --i) {
            int next = neighbors[i];
            if (!visited.count(next)) {
                frontier.push(next);
                if (came_from.find(next) == came_from.end())
                {
                    came_from[next] = current;
                }
            }
        }
    }

    std::vector<int> path;
    int node = goal;
    while (node != start) {
        path.push_back(node);
        node = came_from[node];
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<int> bfs(const std::vector<std::vector<int>>& graph, int start, int goal)
{
    std::unordered_map<int, int> came_from;
    came_from[start] = start;

    std::queue<int> frontier;
    frontier.push(start);
    
    while (!frontier.empty()) {
        int current = frontier.front();
        frontier.pop();

        if (current == goal) break;

        for (int next: graph[current]) {
            if (came_from.find(next) == came_from.end()) {
                came_from[next] = current;
                frontier.push(next);
            }
        }
    }

    std::vector<int> path;
    int node = goal;
    while (node != start) {
        path.push_back(node);
        node = came_from[node];
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;

}

int main() {
    int num_nodes = 7;
    std::vector<std::vector<int>> graph(num_nodes);

    // 0 connects to 1 and 2
    graph[0].push_back(1); graph[1].push_back(0);
    graph[0].push_back(2); graph[2].push_back(0);

    // 1 goes to 3 — dead end
    graph[1].push_back(3); graph[3].push_back(1);

    // 2 goes to 4, 4 goes to 5, 5 goes to 6 (goal)
    graph[2].push_back(4); graph[4].push_back(2);
    graph[4].push_back(5); graph[5].push_back(4);
    graph[5].push_back(6); graph[6].push_back(5);


    auto result_dfs = dfs(graph, 0, 6);
    for (int node : result_dfs) {
        std::cout << node << " ";
    }
    std::cout << std::endl;

    auto result_bfs = bfs(graph, 0, 6);
    for (int node : result_bfs) {
        std::cout << node << " ";
    }
}