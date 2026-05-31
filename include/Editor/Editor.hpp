#pragma once
#include "../Graph/Graph.hpp"
#include <map>
#include <raylib.h>
#include <string>
#include <vector>

enum class NodeState { NORMAL, START, VISITED, PATH, ACTIVE };
enum class AlgorithmType { NONE, DFS, BFS, DIJKSTRA, BELLMAN_FORD };
enum class EditorMode { SELECT, ADD_NODE, ADD_EDGE, DELETE };
enum class InputField {
  NONE,
  EDGE_SRC,
  EDGE_DST,
  EDGE_WEIGHT,
  START_NODE,
  END_NODE
};

struct AnimNode {
  float scale = 1.0f;
  float pulseT = 0.0f;
  float glowAlpha = 0.0f;
};

struct Particle {
  Vector2 pos;
  Vector2 vel;
  float life;
  float maxLife;
  Color color;
};

struct Notification {
  std::string text;
  float life;
  Color color;
};

struct UILayout {
  int dirBtnY = 0;
  int edgeInputY = 0;
  int addEdgeBtnY = 0;
  int clearBtnY = 0;
  int algoBtnY = 0;
  int startInputY = 0;
  int runBtnY = 0;
  int playbackBtnY = 0;
  int speedBtnY = 0;
  int resetBtnY = 0;
};

class Editor {
  Graph &graph;

  struct NodeData {
    Vector2 pos;
    NodeState state = NodeState::NORMAL;
    AnimNode anim;
    int id;
  };

  std::map<int, NodeData> nodes;
  int nextId = 1;
  int selectedNode = -1;
  int edgeSourceNode = -1;
  bool dragging = false;
  Vector2 dragOffset = {0, 0};

  EditorMode mode = EditorMode::SELECT;
  AlgorithmType currentAlgo = AlgorithmType::NONE;
  InputField focusedInput = InputField::NONE;

  char edgeSrcBuf[8] = "";
  char edgeDstBuf[8] = "";
  char edgeWtBuf[8] = "1";
  char startBuf[8] = "";
  char endBuf[8] = "";
  int edgeSrcIdx = 0, edgeDstIdx = 0, edgeWtIdx = 1;
  int startIdx = 0, endIdx = 0;

  std::vector<StepResult> algoSteps;
  int currentStep = -1;
  float stepTimer = 0.0f;
  float stepDelay = 0.6f;
  bool autoPlay = false;

  std::vector<Particle> particles;
  std::vector<Notification> notifications;

  Camera2D camera = {{0, 0}, {0, 0}, 0.0f, 1.0f};
  bool panning = false;
  Vector2 panStart = {0, 0};
  Vector2 camStartTarget = {0, 0};

  UILayout ui;

  void SpawnParticles(Vector2 pos, Color color, int count);
  void UpdateParticles(float dt);
  void DrawParticles();
  void Notify(const std::string &msg, Color color);
  void DrawNotifications();

  void DrawNode(int id, NodeData &nd);
  void DrawEdges();
  Color GetNodeColor(NodeState state);

  void RunAlgorithm();
  void ApplyStep(int stepIdx);
  void ResetNodeStates();

  bool IsInPanel(Vector2 mouse);
  void HandleTextInput(char *buf, int &idx, int maxLen, bool numbersOnly,
                       bool allowDot);

public:
  Editor(Graph &g);

  void Update();
  void Draw();
  void DrawUI();
  void HandleUI();
  void HandleInput();
  void AddNode(int id, Vector2 position);
};
