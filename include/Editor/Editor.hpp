#pragma once
#include "../Graph/Graph.hpp"
#include <map>
#include <raylib.h>

enum NodeState { NORMAL, ROOT, VISITED, PATH };
enum AlgorithmType { NONE, DFS, BFS, DIJKSTRA, ASTAR, BELLMAN_FORD };
enum InputField {
  NONE_FOCUSED,
  NODE_COUNT_INPUT,
  EDGE_INPUT,
  START_NODE_INPUT
};

class Editor {
  Graph &graph;
  std::unordered_map<int, NodeState> NodeStates;

  struct NodeData {
    Vector2 pos;
  };
  std::map<int, NodeData> nodes;
  int selectedNode = -1;

  char nodeCountBuffer[10] = "\0";
  char edgeBuffer[20] = "\0";
  char startNodeBuffer[10] = "\0";
  int nodeCountIndex = 0;
  int edgeIndex = 0;
  int startNodeIndex = 0;

  InputField focusedInput = NONE_FOCUSED;
  AlgorithmType currentAlgo = NONE;

public:
  Editor(Graph &_graph) : graph(_graph) {}

  void Update();
  void Draw();
  void DrawUI();
  void HandleUI();
  void HandleInput();
  void SetNodeState(int id, NodeState state);
  void AddNode(int id, Vector2 position);
};
