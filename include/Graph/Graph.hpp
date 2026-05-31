#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

struct Edge {
  int to;
  float weight;
};

struct StepResult {
  std::vector<int> visited;
  std::vector<int> path;
  std::vector<std::pair<int, int>> activeEdges;
};

class Graph {
public:
  bool isDirected = false;
  std::unordered_map<int, std::vector<Edge>> adj;

  void AddEdge(int u, int v, float weight = 1.0f);
  void RemoveEdge(int u, int v);
  void RemoveNode(int id);
  void Clear();

  std::vector<StepResult> BfsSteps(int start);
  std::vector<StepResult> DfsSteps(int start);
  std::vector<StepResult> DijkstraSteps(int start, int end);
  std::vector<StepResult> BellmanFordSteps(int start, int end);
};
