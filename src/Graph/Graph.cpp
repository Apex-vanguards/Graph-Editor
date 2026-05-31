#include "../../include/Graph/Graph.hpp"
#include <algorithm>
#include <climits>
#include <queue>
#include <stack>
#include <unordered_set>

void Graph::AddEdge(int u, int v, float weight) {
  adj[u].push_back({v, weight});
  if (!isDirected)
    adj[v].push_back({u, weight});
  if (adj.find(u) == adj.end())
    adj[u] = {};
  if (adj.find(v) == adj.end())
    adj[v] = {};
}

void Graph::RemoveEdge(int u, int v) {
  auto &eu = adj[u];
  eu.erase(std::remove_if(eu.begin(), eu.end(),
                          [v](const Edge &e) { return e.to == v; }),
           eu.end());
  if (!isDirected) {
    auto &ev = adj[v];
    ev.erase(std::remove_if(ev.begin(), ev.end(),
                            [u](const Edge &e) { return e.to == u; }),
             ev.end());
  }
}

void Graph::RemoveNode(int id) {
  adj.erase(id);
  for (auto &[node, edges] : adj) {
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [id](const Edge &e) { return e.to == id; }),
                edges.end());
  }
}

void Graph::Clear() { adj.clear(); }

std::vector<StepResult> Graph::BfsSteps(int start) {
  std::vector<StepResult> steps;
  std::unordered_map<int, bool> visited;
  std::queue<int> q;
  std::vector<int> visitedOrder;

  q.push(start);
  visited[start] = true;

  while (!q.empty()) {
    int node = q.front();
    q.pop();
    visitedOrder.push_back(node);

    StepResult step;
    step.visited = visitedOrder;

    if (adj.count(node)) {
      for (auto &e : adj[node]) {
        if (!visited[e.to]) {
          visited[e.to] = true;
          q.push(e.to);
          step.activeEdges.push_back({node, e.to});
        }
      }
    }
    steps.push_back(step);
  }
  return steps;
}

std::vector<StepResult> Graph::DfsSteps(int start) {
  std::vector<StepResult> steps;
  std::unordered_set<int> visited;
  std::vector<int> visitedOrder;
  std::stack<std::pair<int, int>> s;

  s.push({start, -1});

  while (!s.empty()) {
    auto [node, parent] = s.top();
    s.pop();
    if (visited.count(node))
      continue;
    visited.insert(node);
    visitedOrder.push_back(node);

    StepResult step;
    step.visited = visitedOrder;
    if (parent != -1)
      step.activeEdges.push_back({parent, node});
    steps.push_back(step);

    if (adj.count(node)) {
      for (auto it = adj[node].rbegin(); it != adj[node].rend(); ++it) {
        if (!visited.count(it->to))
          s.push({it->to, node});
      }
    }
  }
  return steps;
}

std::vector<StepResult> Graph::DijkstraSteps(int start, int end) {
  std::vector<StepResult> steps;
  std::unordered_map<int, float> dist;
  std::unordered_map<int, int> prev;
  std::unordered_set<int> visited;

  for (auto &[n, _] : adj)
    dist[n] = 1e9f;
  dist[start] = 0.0f;

  using P = std::pair<float, int>;
  std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
  pq.push({0.0f, start});

  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();

    if (visited.count(u))
      continue;
    visited.insert(u);

    StepResult step;
    for (auto v : visited)
      step.visited.push_back(v);

    if (adj.count(u)) {
      for (auto &e : adj[u]) {
        step.activeEdges.push_back({u, e.to});
        if (!visited.count(e.to) && dist[u] + e.weight < dist[e.to]) {
          dist[e.to] = dist[u] + e.weight;
          prev[e.to] = u;
          pq.push({dist[e.to], e.to});
        }
      }
    }
    steps.push_back(step);

    if (u == end)
      break;
  }

  if (!steps.empty()) {
    StepResult &last = steps.back();
    int cur = end;
    while (prev.count(cur)) {
      last.path.push_back(cur);
      cur = prev[cur];
    }
    last.path.push_back(start);
    std::reverse(last.path.begin(), last.path.end());
  }
  return steps;
}

std::vector<StepResult> Graph::BellmanFordSteps(int start, int end) {
  std::vector<StepResult> steps;
  std::unordered_map<int, float> dist;
  std::unordered_map<int, int> prev;

  for (auto &[n, _] : adj)
    dist[n] = 1e9f;
  dist[start] = 0.0f;

  int V = (int)adj.size();
  for (int i = 0; i < V - 1; i++) {
    StepResult step;
    bool changed = false;
    for (auto &[u, edges] : adj) {
      if (dist[u] >= 1e9f)
        continue;
      for (auto &e : edges) {
        step.activeEdges.push_back({u, e.to});
        if (dist[u] + e.weight < dist[e.to]) {
          dist[e.to] = dist[u] + e.weight;
          prev[e.to] = u;
          changed = true;
        }
      }
    }
    for (auto &[n, d] : dist)
      if (d < 1e9f)
        step.visited.push_back(n);
    steps.push_back(step);
    if (!changed)
      break;
  }

  if (!steps.empty()) {
    StepResult &last = steps.back();
    int cur = end;
    while (prev.count(cur) && cur != start) {
      last.path.push_back(cur);
      cur = prev[cur];
    }
    last.path.push_back(start);
    std::reverse(last.path.begin(), last.path.end());
  }
  return steps;
}
