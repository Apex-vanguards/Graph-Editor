#pragma once
#include <unordered_map>
#include <vector>

struct Edge {
  int to;
  float weight;
};

class Graph {
public:
  bool isDirected = false;
  std::unordered_map<int, std::vector<Edge>> adj;

  void AddEdge(int u, int v, float weight = 1.0f);

  void Bfs(int start);
  void Dijkstra(int start, int end);
};
