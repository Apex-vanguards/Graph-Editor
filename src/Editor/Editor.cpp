#include "../../include/Editor/Editor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <set>
#include <sstream>

static const int PANEL_W = 320;
static const int SCREEN_H = 720;
static const float NODE_R = 22.0f;

static Color C_PANEL = {16, 18, 30, 255};
static Color C_PANEL2 = {22, 25, 42, 255};
static Color C_ACCENT = {94, 234, 212, 255};
static Color C_ACCENT2 = {251, 113, 133, 255};
static Color C_GOLD = {250, 204, 21, 255};
static Color C_PURPLE = {167, 139, 250, 255};
static Color C_TEXT = {226, 232, 240, 255};
static Color C_MUTED = {100, 116, 139, 255};
static Color C_NODE_NORM = {30, 41, 59, 255};
static Color C_VISITED = {56, 189, 248, 255};
static Color C_PATH = {250, 204, 21, 255};
static Color C_START = {94, 234, 212, 255};
static Color C_ACTIVE = {251, 113, 133, 255};
static Color C_EDGE = {51, 65, 85, 255};
static Color C_EDGE_ACT = {251, 191, 36, 255};
static Color C_PATH_EDGE = {250, 204, 21, 255};

static Color Lerp4(Color a, Color b, float t) {
  return {(unsigned char)(a.r + (b.r - a.r) * t),
          (unsigned char)(a.g + (b.g - a.g) * t),
          (unsigned char)(a.b + (b.b - a.b) * t),
          (unsigned char)(a.a + (b.a - a.a) * t)};
}

static Color WithAlpha(Color c, unsigned char a) {
  c.a = a;
  return c;
}

static float Dist2(Vector2 a, Vector2 b) {
  float dx = a.x - b.x, dy = a.y - b.y;
  return dx * dx + dy * dy;
}

static bool ButtonClicked(Rectangle r, Vector2 mouse) {
  return CheckCollisionPointRec(mouse, r) &&
         IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

static void DrawLabel(const char *text, int x, int y) {
  DrawText(text, x, y, 13, {100, 116, 139, 255});
}

static void DrawInputBox(Rectangle r, const char *buf, bool focused,
                         Color accent) {
  DrawRectangleRounded(r, 0.3f, 8, {22, 25, 42, 255});
  DrawRectangleRoundedLines(r, 0.3f, 8, 1.5f,
                            focused ? accent : (Color){40, 50, 72, 255});
  DrawText(buf, (int)r.x + 8, (int)(r.y + r.height / 2) - 8, 16,
           focused ? C_TEXT : (Color){148, 163, 184, 255});
  if (focused) {
    float t = sinf((float)GetTime() * 4.0f);
    if (t > 0) {
      int tw = MeasureText(buf, 16);
      DrawRectangle((int)r.x + 8 + tw, (int)(r.y + r.height / 2) - 8, 2, 16,
                    accent);
    }
  }
}

static void DrawButton(Rectangle r, const char *text, Color bg, Color textColor,
                       bool hovered) {
  Color c = hovered ? Lerp4(bg, {255, 255, 255, 255}, 0.15f) : bg;
  DrawRectangleRounded(r, 0.35f, 10, c);
  if (hovered)
    DrawRectangleRoundedLines(r, 0.35f, 10, 1.5f,
                              WithAlpha({255, 255, 255, 255}, 80));
  int tw = MeasureText(text, 15);
  DrawText(text, (int)(r.x + r.width / 2 - tw / 2),
           (int)(r.y + r.height / 2 - 8), 15, textColor);
}

Editor::Editor(Graph &g) : graph(g) {
  camera.zoom = 1.0f;
  camera.offset = {(float)(GetScreenWidth() - PANEL_W) / 2 + PANEL_W,
                   (float)SCREEN_H / 2};
  camera.target = {0, 0};
}

void Editor::AddNode(int id, Vector2 position) {
  nodes[id] = {position, NodeState::NORMAL, {}, id};
  if (id >= nextId)
    nextId = id + 1;
  graph.adj[id];
}

void Editor::SpawnParticles(Vector2 pos, Color color, int count) {
  for (int i = 0; i < count; i++) {
    float angle = ((float)rand() / RAND_MAX) * 6.283f;
    float speed = 40.0f + ((float)rand() / RAND_MAX) * 80.0f;
    particles.push_back(
        {pos, {cosf(angle) * speed, sinf(angle) * speed}, 0.8f, 0.8f, color});
  }
}

void Editor::UpdateParticles(float dt) {
  for (auto &p : particles) {
    p.pos.x += p.vel.x * dt;
    p.pos.y += p.vel.y * dt;
    p.vel.x *= 0.95f;
    p.vel.y *= 0.95f;
    p.life -= dt;
  }
  particles.erase(std::remove_if(particles.begin(), particles.end(),
                                 [](const Particle &p) { return p.life <= 0; }),
                  particles.end());
}

void Editor::DrawParticles() {
  for (auto &p : particles) {
    float t = p.life / p.maxLife;
    Color c = WithAlpha(p.color, (unsigned char)(t * 255));
    DrawCircleV(p.pos, 3.0f * t, c);
  }
}

void Editor::Notify(const std::string &msg, Color color) {
  notifications.push_back({msg, 2.5f, color});
  if (notifications.size() > 4)
    notifications.erase(notifications.begin());
}

void Editor::DrawNotifications() {
  float ny = 20.0f;
  for (auto &n : notifications) {
    float alpha = fminf(n.life, 0.3f) / 0.3f * fminf(n.life, 0.5f) / 0.5f;
    Color c = WithAlpha(n.color, (unsigned char)(alpha * 230));
    Color bg = WithAlpha(C_PANEL, (unsigned char)(alpha * 200));
    float tw = (float)MeasureText(n.text.c_str(), 16);
    DrawRectangleRounded(
        {(float)(GetScreenWidth() - PANEL_W) - tw - 30, ny - 6, tw + 20, 28},
        0.4f, 8, bg);
    DrawText(n.text.c_str(), (int)((GetScreenWidth() - PANEL_W) - tw - 20),
             (int)ny, 16, c);
    ny += 36;
  }
}

bool Editor::IsInPanel(Vector2 mouse) { return mouse.x < PANEL_W; }

Color Editor::GetNodeColor(NodeState state) {
  switch (state) {
  case NodeState::START:
    return C_START;
  case NodeState::VISITED:
    return C_VISITED;
  case NodeState::PATH:
    return C_PATH;
  case NodeState::ACTIVE:
    return C_ACTIVE;
  default:
    return C_NODE_NORM;
  }
}

void Editor::DrawNode(int id, NodeData &nd) {
  nd.anim.pulseT += GetFrameTime() * 2.0f;
  float pulse = sinf(nd.anim.pulseT) * 0.5f + 0.5f;
  Color base = GetNodeColor(nd.state);
  float sel = (selectedNode == id || edgeSourceNode == id) ? 1.0f : 0.0f;
  Vector2 wp = GetWorldToScreen2D(nd.pos, camera);

  if (nd.state != NodeState::NORMAL) {
    DrawCircleV(wp, NODE_R + 5.0f + pulse * 4.0f, WithAlpha(base, 40));
    DrawCircleV(wp, NODE_R + 5.0f, WithAlpha(base, 80));
  }
  if (sel > 0.5f)
    DrawCircleV(wp, NODE_R + 6.0f, WithAlpha(C_ACCENT, 160));

  DrawCircleV(
      wp, NODE_R,
      Lerp4(C_NODE_NORM, base, nd.state == NodeState::NORMAL ? sel : 1.0f));
  DrawCircleV(wp, NODE_R - 3.0f,
              WithAlpha(base, nd.state == NodeState::NORMAL
                                  ? (unsigned char)(sel * 60)
                                  : 30));
  DrawCircleV(wp, NODE_R - 8.0f,
              WithAlpha(nd.state == NodeState::NORMAL ? C_ACCENT : base, 60));
  DrawRingLines(wp, NODE_R - 1, NODE_R + 1, 0, 360, 36,
                nd.state == NodeState::NORMAL ? C_MUTED : WithAlpha(base, 200));

  const char *label = TextFormat("%d", id);
  int tw = MeasureText(label, 18);
  DrawText(label, (int)(wp.x - tw / 2), (int)(wp.y - 9), 18, C_TEXT);
}

static void DrawArrowHead(Vector2 from, Vector2 to, Color color) {
  float dx = to.x - from.x, dy = to.y - from.y;
  float len = sqrtf(dx * dx + dy * dy);
  if (len < 1.0f)
    return;
  dx /= len;
  dy /= len;
  float px = -dy, py = dx;
  float arrowLen = 14.0f, arrowW = 7.0f;
  Vector2 tip = {to.x - dx * NODE_R, to.y - dy * NODE_R};
  Vector2 base1 = {tip.x - dx * arrowLen + px * arrowW,
                   tip.y - dy * arrowLen + py * arrowW};
  Vector2 base2 = {tip.x - dx * arrowLen - px * arrowW,
                   tip.y - dy * arrowLen - py * arrowW};
  DrawTriangle(tip, base1, base2, color);
}

void Editor::DrawEdges() {
  std::set<std::pair<int, int>> drawn;
  for (auto &[u, edges] : graph.adj) {
    if (!nodes.count(u))
      continue;
    for (auto &e : edges) {
      if (!nodes.count(e.to))
        continue;
      auto key = std::make_pair(std::min(u, e.to), std::max(u, e.to));
      if (!graph.isDirected && drawn.count(key))
        continue;
      drawn.insert(key);

      Vector2 wp1 = GetWorldToScreen2D(nodes[u].pos, camera);
      Vector2 wp2 = GetWorldToScreen2D(nodes[e.to].pos, camera);

      Color col = C_EDGE;
      bool active = false;
      bool onPath = false;

      if (currentStep >= 0 && currentStep < (int)algoSteps.size()) {
        auto &step = algoSteps[currentStep];
        for (auto &ae : step.activeEdges) {
          if ((ae.first == u && ae.second == e.to) ||
              (!graph.isDirected && ae.first == e.to && ae.second == u)) {
            col = C_EDGE_ACT;
            active = true;
            break;
          }
        }
        auto &path = step.path;
        for (int i = 0; i + 1 < (int)path.size(); i++) {
          if ((path[i] == u && path[i + 1] == e.to) ||
              (!graph.isDirected && path[i] == e.to && path[i + 1] == u)) {
            col = C_PATH_EDGE;
            onPath = true;
            break;
          }
        }
      }

      float thickness = (active || onPath) ? 3.0f : 1.5f;
      DrawLineEx(wp1, wp2, thickness, col);
      if (graph.isDirected)
        DrawArrowHead(wp1, wp2, col);

      if (e.weight != 1.0f) {
        Vector2 mid = {(wp1.x + wp2.x) / 2, (wp1.y + wp2.y) / 2};
        const char *wl = TextFormat("%.0f", e.weight);
        int wtw = MeasureText(wl, 12);
        DrawRectangle((int)(mid.x - wtw / 2 - 3), (int)(mid.y - 9), wtw + 6, 18,
                      WithAlpha(C_PANEL, 200));
        DrawText(wl, (int)(mid.x - wtw / 2), (int)(mid.y - 6), 12, col);
      }
    }
  }
}

void Editor::ResetNodeStates() {
  for (auto &[id, nd] : nodes)
    nd.state = NodeState::NORMAL;
}

void Editor::ApplyStep(int stepIdx) {
  ResetNodeStates();
  if (stepIdx < 0 || stepIdx >= (int)algoSteps.size())
    return;
  auto &step = algoSteps[stepIdx];

  for (int v : step.visited)
    if (nodes.count(v))
      nodes[v].state = NodeState::VISITED;

  for (int v : step.path)
    if (nodes.count(v))
      nodes[v].state = NodeState::PATH;

  if (!step.visited.empty()) {
    int last = step.visited.back();
    if (nodes.count(last) && nodes[last].state == NodeState::VISITED)
      nodes[last].state = NodeState::ACTIVE;
  }

  if (startIdx > 0) {
    int sid = atoi(startBuf);
    if (nodes.count(sid) && nodes[sid].state != NodeState::PATH)
      nodes[sid].state = NodeState::START;
  }
}

void Editor::RunAlgorithm() {
  algoSteps.clear();
  currentStep = -1;
  ResetNodeStates();

  if (startIdx == 0) {
    Notify("Start node girmedin!", C_ACCENT2);
    return;
  }
  int startId = atoi(startBuf);
  if (!nodes.count(startId)) {
    Notify("Start node tapilmadi: " + std::string(startBuf), C_ACCENT2);
    return;
  }

  bool needEnd = currentAlgo == AlgorithmType::DIJKSTRA ||
                 currentAlgo == AlgorithmType::BELLMAN_FORD;
  int endId = -1;
  if (needEnd) {
    if (endIdx == 0) {
      Notify("End node girmedin!", C_ACCENT2);
      return;
    }
    endId = atoi(endBuf);
    if (!nodes.count(endId)) {
      Notify("End node tapilmadi: " + std::string(endBuf), C_ACCENT2);
      return;
    }
  }

  switch (currentAlgo) {
  case AlgorithmType::BFS:
    algoSteps = graph.BfsSteps(startId);
    Notify("BFS: node " + std::to_string(startId) + "-den bashladi", C_ACCENT);
    break;
  case AlgorithmType::DFS:
    algoSteps = graph.DfsSteps(startId);
    Notify("DFS: node " + std::to_string(startId) + "-den bashladi", C_PURPLE);
    break;
  case AlgorithmType::DIJKSTRA:
    algoSteps = graph.DijkstraSteps(startId, endId);
    Notify("Dijkstra: " + std::to_string(startId) + " -> " +
               std::to_string(endId),
           C_GOLD);
    break;
  case AlgorithmType::BELLMAN_FORD:
    algoSteps = graph.BellmanFordSteps(startId, endId);
    Notify("Bellman-Ford: " + std::to_string(startId) + " -> " +
               std::to_string(endId),
           C_ACCENT2);
    break;
  default:
    Notify("Algo sech!", C_MUTED);
    return;
  }

  if (algoSteps.empty()) {
    Notify("Netice yoxdur (node baghlimi?)", C_ACCENT2);
    return;
  }
  currentStep = 0;
  ApplyStep(0);
  autoPlay = true;
  stepTimer = 0.0f;
}

void Editor::HandleTextInput(char *buf, int &idx, int maxLen, bool numbersOnly,
                             bool allowDot) {
  int key = GetCharPressed();
  while (key > 0) {
    bool ok = false;
    if (numbersOnly)
      ok = (key >= '0' && key <= '9') || (allowDot && key == '.');
    else
      ok = (key >= 32 && key <= 125);
    if (ok && idx < maxLen - 1) {
      buf[idx++] = (char)key;
      buf[idx] = '\0';
    }
    key = GetCharPressed();
  }
  if (IsKeyPressed(KEY_BACKSPACE) && idx > 0)
    buf[--idx] = '\0';
}

void Editor::DrawUI() {
  DrawRectangle(0, 0, PANEL_W, SCREEN_H, C_PANEL);
  DrawRectangle(PANEL_W - 1, 0, 1, SCREEN_H, WithAlpha(C_ACCENT, 40));
  DrawRectangle(0, 0, PANEL_W, 56, C_PANEL2);
  DrawText("GRAPH EDITOR", 18, 10, 20, C_ACCENT);
  DrawText("v2.1", 18, 32, 12, C_MUTED);

  Vector2 mouse = GetMousePosition();

  const char *modeLabels[] = {"SELECT", "ADD NODE", "ADD EDGE", "DELETE"};
  Color modeColors[] = {C_MUTED, C_ACCENT, C_PURPLE, C_ACCENT2};
  float btnW = (PANEL_W - 20 - 18.0f) / 4.0f;
  for (int i = 0; i < 4; i++) {
    Rectangle r = {10.0f + i * (btnW + 6), 66, btnW, 28};
    bool isActive = (int)mode == i;
    Color bg = isActive ? modeColors[i] : C_PANEL2;
    DrawRectangleRounded(r, 0.3f, 8, bg);
    if (!isActive)
      DrawRectangleRoundedLines(r, 0.3f, 8, 1.0f, (Color){40, 50, 72, 255});
    int tw = MeasureText(modeLabels[i], 11);
    DrawText(modeLabels[i], (int)(r.x + r.width / 2 - tw / 2), (int)(r.y + 8),
             11, isActive ? C_PANEL : C_MUTED);
  }

  int y = 108;
  DrawRectangle(0, y, PANEL_W, 1, {40, 50, 72, 255});
  y += 10;
  DrawText("GRAPH SETTINGS", 14, y, 12, C_MUTED);
  y += 20;

  ui.dirBtnY = y;
  bool isDirHov = CheckCollisionPointRec(mouse, {10, (float)y, 130, 28});
  bool isUndHov = CheckCollisionPointRec(mouse, {148, (float)y, 130, 28});
  DrawButton({10, (float)y, 130, 28}, "Directed",
             graph.isDirected ? C_PURPLE : C_PANEL2,
             graph.isDirected ? C_PANEL : C_TEXT, isDirHov);
  DrawButton({148, (float)y, 130, 28}, "Undirected",
             !graph.isDirected ? C_ACCENT : C_PANEL2,
             !graph.isDirected ? C_PANEL : C_TEXT, isUndHov);
  y += 40;

  DrawRectangle(0, y, PANEL_W, 1, {40, 50, 72, 255});
  y += 10;
  DrawText("ADD EDGE", 14, y, 12, C_MUTED);
  y += 20;

  DrawLabel("From", 14, y);
  DrawLabel("To", 112, y);
  DrawLabel("Weight", 210, y);
  y += 15;
  ui.edgeInputY = y;
  DrawInputBox({10, (float)y, 90, 32}, edgeSrcBuf,
               focusedInput == InputField::EDGE_SRC, C_PURPLE);
  DrawInputBox({108, (float)y, 90, 32}, edgeDstBuf,
               focusedInput == InputField::EDGE_DST, C_PURPLE);
  DrawInputBox({206, (float)y, 90, 32}, edgeWtBuf,
               focusedInput == InputField::EDGE_WEIGHT, C_PURPLE);
  y += 42;

  ui.addEdgeBtnY = y;
  bool addEdgeHov =
      CheckCollisionPointRec(mouse, {10, (float)y, PANEL_W - 20, 30});
  DrawButton({10, (float)y, PANEL_W - 20, 30}, "+ ADD EDGE", C_PURPLE, C_TEXT,
             addEdgeHov);
  y += 42;

  ui.clearBtnY = y;
  float chW = (PANEL_W - 26) / 2.0f;
  bool clearHov = CheckCollisionPointRec(mouse, {10, (float)y, chW, 28});
  bool clearAllHov =
      CheckCollisionPointRec(mouse, {10 + chW + 6, (float)y, chW, 28});
  DrawButton({10, (float)y, chW, 28}, "Clear Edges", {30, 41, 59, 255}, C_TEXT,
             clearHov);
  DrawButton({10 + chW + 6, (float)y, chW, 28}, "Clear All",
             WithAlpha(C_ACCENT2, 180), C_TEXT, clearAllHov);
  y += 44;

  DrawRectangle(0, y, PANEL_W, 1, {40, 50, 72, 255});
  y += 10;
  DrawText("ALGORITHM", 14, y, 12, C_MUTED);
  y += 20;

  struct AlgoInfo {
    const char *name;
    AlgorithmType type;
    Color color;
  };
  AlgoInfo algos[] = {
      {"BFS", AlgorithmType::BFS, C_ACCENT},
      {"DFS", AlgorithmType::DFS, C_PURPLE},
      {"Dijkstra", AlgorithmType::DIJKSTRA, C_GOLD},
      {"Bellman-Ford", AlgorithmType::BELLMAN_FORD, C_ACCENT2},
  };
  float aw = (PANEL_W - 20 - 6) / 2.0f;
  ui.algoBtnY = y;
  for (int i = 0; i < 4; i++) {
    float ax = 10 + (i % 2) * (aw + 6);
    float ay = (float)y + (i / 2) * 36;
    bool isActive = currentAlgo == algos[i].type;
    bool hov = CheckCollisionPointRec(mouse, {ax, ay, aw, 28});
    DrawButton({ax, ay, aw, 28}, algos[i].name,
               isActive ? algos[i].color : C_PANEL2,
               isActive ? C_PANEL : C_TEXT, hov);
  }
  y += 80;

  bool needEnd = currentAlgo == AlgorithmType::DIJKSTRA ||
                 currentAlgo == AlgorithmType::BELLMAN_FORD;
  DrawLabel("Start Node", 14, y);
  if (needEnd)
    DrawLabel("End Node", 170, y);
  y += 15;
  ui.startInputY = y;
  DrawInputBox({10, (float)y, needEnd ? 140.0f : (float)(PANEL_W - 20), 32},
               startBuf, focusedInput == InputField::START_NODE, C_ACCENT);
  if (needEnd)
    DrawInputBox({158, (float)y, 140, 32}, endBuf,
                 focusedInput == InputField::END_NODE, C_ACCENT2);
  y += 42;

  ui.runBtnY = y;
  bool runHov = CheckCollisionPointRec(mouse, {10, (float)y, PANEL_W - 20, 34});
  DrawButton({10, (float)y, PANEL_W - 20, 34}, "▶  RUN ALGORITHM",
             currentAlgo != AlgorithmType::NONE ? C_ACCENT : C_PANEL2,
             currentAlgo != AlgorithmType::NONE ? C_PANEL : C_MUTED, runHov);
  y += 46;

  if (!algoSteps.empty()) {
    DrawRectangle(0, y, PANEL_W, 1, {40, 50, 72, 255});
    y += 10;
    DrawText("PLAYBACK", 14, y, 12, C_MUTED);
    y += 18;

    float prog = (float)(currentStep + 1) / (float)algoSteps.size();
    DrawRectangleRounded({10, (float)y, PANEL_W - 20, 8}, 0.5f, 8, C_PANEL2);
    if (prog > 0)
      DrawRectangleRounded({10, (float)y, (PANEL_W - 20) * prog, 8}, 0.5f, 8,
                           C_ACCENT);
    y += 20;

    DrawText(TextFormat("Step %d / %d", currentStep + 1, (int)algoSteps.size()),
             14, y, 14, C_MUTED);
    y += 22;

    float bw = (PANEL_W - 26) / 3.0f;
    ui.playbackBtnY = y;
    DrawButton({10, (float)y, bw, 28}, "◀ Prev", C_PANEL2, C_TEXT,
               CheckCollisionPointRec(mouse, {10, (float)y, bw, 28}));
    DrawButton({10 + bw + 3, (float)y, bw, 28}, autoPlay ? "⏸ Pause" : "▶ Play",
               autoPlay ? C_ACCENT2 : C_ACCENT, C_PANEL,
               CheckCollisionPointRec(mouse, {10 + bw + 3, (float)y, bw, 28}));
    DrawButton(
        {10 + bw * 2 + 6, (float)y, bw, 28}, "Next ▶", C_PANEL2, C_TEXT,
        CheckCollisionPointRec(mouse, {10 + bw * 2 + 6, (float)y, bw, 28}));
    y += 40;

    DrawLabel("Speed", 14, y);
    y += 15;
    DrawRectangleRounded({10, (float)y, PANEL_W - 20, 8}, 0.5f, 8, C_PANEL2);
    float speedNorm = 1.0f - (stepDelay - 0.1f) / 1.4f;
    DrawRectangleRounded({10, (float)y, (PANEL_W - 20) * speedNorm, 8}, 0.5f, 8,
                         C_GOLD);
    y += 26;

    ui.speedBtnY = y;
    DrawButton({10, (float)y, bw * 1.5f, 24}, "Faster ▲", C_PANEL2, C_TEXT,
               CheckCollisionPointRec(mouse, {10, (float)y, bw * 1.5f, 24}));
    DrawButton({10 + bw * 1.5f + 6, (float)y, bw * 1.5f, 24}, "Slower ▼",
               C_PANEL2, C_TEXT,
               CheckCollisionPointRec(
                   mouse, {10 + bw * 1.5f + 6, (float)y, bw * 1.5f, 24}));
    y += 36;

    ui.resetBtnY = y;
    DrawButton({10, (float)y, PANEL_W - 20, 26}, "Reset", C_PANEL2, C_TEXT,
               CheckCollisionPointRec(mouse, {10, (float)y, PANEL_W - 20, 26}));
  }

  DrawRectangle(0, SCREEN_H - 40, PANEL_W, 40, C_PANEL2);
  const char *hints[] = {
      "Click node to select/drag", "Click canvas to add node",
      "Click src node, then dst node", "Click node to delete"};
  DrawText(hints[(int)mode], 12, SCREEN_H - 28, 12, C_MUTED);
}

void Editor::HandleUI() {
  Vector2 mouse = GetMousePosition();
  if (!IsInPanel(mouse))
    return;

  float btnW = (PANEL_W - 20 - 18.0f) / 4.0f;
  for (int i = 0; i < 4; i++) {
    if (ButtonClicked({10 + i * (btnW + 6), 66, btnW, 28}, mouse))
      mode = (EditorMode)i;
  }

  if (ButtonClicked({10, (float)ui.dirBtnY, 130, 28}, mouse)) {
    graph.isDirected = true;
    Notify("Directed mode", C_PURPLE);
  }
  if (ButtonClicked({148, (float)ui.dirBtnY, 130, 28}, mouse)) {
    graph.isDirected = false;
    Notify("Undirected mode", C_ACCENT);
  }

  if (CheckCollisionPointRec(mouse, {10, (float)ui.edgeInputY, 90, 32}) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = InputField::EDGE_SRC;
  else if (CheckCollisionPointRec(mouse, {108, (float)ui.edgeInputY, 90, 32}) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = InputField::EDGE_DST;
  else if (CheckCollisionPointRec(mouse, {206, (float)ui.edgeInputY, 90, 32}) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = InputField::EDGE_WEIGHT;

  if (ButtonClicked({10, (float)ui.addEdgeBtnY, PANEL_W - 20, 30}, mouse)) {
    if (edgeSrcIdx == 0 || edgeDstIdx == 0) {
      Notify("From ve To node daxil et", C_ACCENT2);
    } else {
      int u = atoi(edgeSrcBuf), v = atoi(edgeDstBuf);
      float w = edgeWtIdx > 0 ? (float)atof(edgeWtBuf) : 1.0f;
      if (nodes.count(u) && nodes.count(v) && u != v) {
        graph.AddEdge(u, v, w);
        SpawnParticles(nodes[u].pos, C_PURPLE, 8);
        Notify("Edge " + std::to_string(u) + " -> " + std::to_string(v) +
                   " eklendi",
               C_PURPLE);
      } else if (u == v) {
        Notify("Self-loop olmaz", C_ACCENT2);
      } else {
        Notify("Node " + std::to_string(u) + " ve ya " + std::to_string(v) +
                   " yoxdur",
               C_ACCENT2);
      }
    }
  }

  float chW = (PANEL_W - 26) / 2.0f;
  if (ButtonClicked({10, (float)ui.clearBtnY, chW, 28}, mouse)) {
    graph.adj.clear();
    for (auto &[id, _] : nodes)
      graph.adj[id];
    algoSteps.clear();
    currentStep = -1;
    ResetNodeStates();
    Notify("Edgeler silindi", C_MUTED);
  }
  if (ButtonClicked({10 + chW + 6, (float)ui.clearBtnY, chW, 28}, mouse)) {
    graph.Clear();
    nodes.clear();
    nextId = 1;
    algoSteps.clear();
    currentStep = -1;
    autoPlay = false;
    Notify("Graf silindi", C_ACCENT2);
  }

  AlgorithmType algoList[] = {AlgorithmType::BFS, AlgorithmType::DFS,
                              AlgorithmType::DIJKSTRA,
                              AlgorithmType::BELLMAN_FORD};
  float aw = (PANEL_W - 20 - 6) / 2.0f;
  for (int i = 0; i < 4; i++) {
    float ax = 10 + (i % 2) * (aw + 6);
    float ay = (float)ui.algoBtnY + (i / 2) * 36;
    if (ButtonClicked({ax, ay, aw, 28}, mouse)) {
      currentAlgo = algoList[i];
      algoSteps.clear();
      currentStep = -1;
      ResetNodeStates();
    }
  }

  bool needEnd = currentAlgo == AlgorithmType::DIJKSTRA ||
                 currentAlgo == AlgorithmType::BELLMAN_FORD;
  if (CheckCollisionPointRec(mouse,
                             {10, (float)ui.startInputY,
                              needEnd ? 140.0f : (float)(PANEL_W - 20), 32}) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = InputField::START_NODE;
  if (needEnd &&
      CheckCollisionPointRec(mouse, {158, (float)ui.startInputY, 140, 32}) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = InputField::END_NODE;

  if (ButtonClicked({10, (float)ui.runBtnY, PANEL_W - 20, 34}, mouse))
    RunAlgorithm();

  if (!algoSteps.empty()) {
    float bw = (PANEL_W - 26) / 3.0f;
    if (ButtonClicked({10, (float)ui.playbackBtnY, bw, 28}, mouse) &&
        currentStep > 0) {
      currentStep--;
      ApplyStep(currentStep);
      autoPlay = false;
    }
    if (ButtonClicked({10 + bw + 3, (float)ui.playbackBtnY, bw, 28}, mouse))
      autoPlay = !autoPlay;
    if (ButtonClicked({10 + bw * 2 + 6, (float)ui.playbackBtnY, bw, 28},
                      mouse) &&
        currentStep < (int)algoSteps.size() - 1) {
      currentStep++;
      ApplyStep(currentStep);
    }

    if (ButtonClicked({10, (float)ui.speedBtnY, bw * 1.5f, 24}, mouse))
      stepDelay = fmaxf(0.1f, stepDelay - 0.15f);
    if (ButtonClicked({10 + bw * 1.5f + 6, (float)ui.speedBtnY, bw * 1.5f, 24},
                      mouse))
      stepDelay = fminf(1.5f, stepDelay + 0.15f);

    if (ButtonClicked({10, (float)ui.resetBtnY, PANEL_W - 20, 26}, mouse)) {
      currentStep = -1;
      algoSteps.clear();
      ResetNodeStates();
      autoPlay = false;
    }
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Rectangle edgeSrcR = {10, (float)ui.edgeInputY, 90, 32};
    Rectangle edgeDstR = {108, (float)ui.edgeInputY, 90, 32};
    Rectangle edgeWtR = {206, (float)ui.edgeInputY, 90, 32};
    Rectangle startR = {10, (float)ui.startInputY,
                        needEnd ? 140.0f : (float)(PANEL_W - 20), 32};
    Rectangle endR = {158, (float)ui.startInputY, 140, 32};
    if (!CheckCollisionPointRec(mouse, edgeSrcR) &&
        !CheckCollisionPointRec(mouse, edgeDstR) &&
        !CheckCollisionPointRec(mouse, edgeWtR) &&
        !CheckCollisionPointRec(mouse, startR) &&
        !CheckCollisionPointRec(mouse, endR))
      focusedInput = InputField::NONE;
  }
}

void Editor::HandleInput() {
  switch (focusedInput) {
  case InputField::EDGE_SRC:
    HandleTextInput(edgeSrcBuf, edgeSrcIdx, 7, true, false);
    break;
  case InputField::EDGE_DST:
    HandleTextInput(edgeDstBuf, edgeDstIdx, 7, true, false);
    break;
  case InputField::EDGE_WEIGHT:
    HandleTextInput(edgeWtBuf, edgeWtIdx, 7, true, true);
    break;
  case InputField::START_NODE:
    HandleTextInput(startBuf, startIdx, 7, true, false);
    break;
  case InputField::END_NODE:
    HandleTextInput(endBuf, endIdx, 7, true, false);
    break;
  default:
    break;
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    focusedInput = InputField::NONE;
    mode = EditorMode::SELECT;
  }
  if (IsKeyPressed(KEY_ENTER) && focusedInput != InputField::NONE)
    focusedInput = InputField::NONE;

  if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) &&
      focusedInput == InputField::NONE && selectedNode != -1) {
    graph.RemoveNode(selectedNode);
    nodes.erase(selectedNode);
    SpawnParticles({(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2},
                   C_ACCENT2, 6);
    Notify("Node " + std::to_string(selectedNode) + " silindi", C_ACCENT2);
    selectedNode = -1;
    algoSteps.clear();
    currentStep = -1;
  }

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mp = GetMousePosition();
    if (!IsInPanel(mp)) {
      Vector2 before = GetScreenToWorld2D(mp, camera);
      camera.zoom =
          fmaxf(0.2f, fminf(5.0f, camera.zoom * (1.0f + wheel * 0.1f)));
      Vector2 after = GetScreenToWorld2D(mp, camera);
      camera.target.x += before.x - after.x;
      camera.target.y += before.y - after.y;
    }
  }
  if (IsKeyPressed(KEY_R)) {
    camera.zoom = 1.0f;
    camera.target = {0, 0};
  }
}

void Editor::Update() {
  float dt = GetFrameTime();
  UpdateParticles(dt);
  for (auto &n : notifications)
    n.life -= dt;
  notifications.erase(
      std::remove_if(notifications.begin(), notifications.end(),
                     [](const Notification &n) { return n.life <= 0; }),
      notifications.end());

  if (autoPlay && !algoSteps.empty()) {
    stepTimer += dt;
    if (stepTimer >= stepDelay) {
      stepTimer = 0.0f;
      if (currentStep < (int)algoSteps.size() - 1) {
        currentStep++;
        ApplyStep(currentStep);
        auto &step = algoSteps[currentStep];
        if (!step.visited.empty() && nodes.count(step.visited.back()))
          SpawnParticles(
              GetWorldToScreen2D(nodes[step.visited.back()].pos, camera),
              C_ACCENT, 5);
      } else {
        autoPlay = false;
        auto &last = algoSteps.back();
        if (!last.path.empty())
          Notify("Shortest path tapildi! " +
                     std::to_string((int)last.path.size()) + " node",
                 C_GOLD);
        else
          Notify("Algoritm tamamlandi!", C_GOLD);
      }
    }
  }

  Vector2 mousePos = GetMousePosition();
  if (IsInPanel(mousePos))
    return;
  Vector2 worldMouse = GetScreenToWorld2D(mousePos, camera);

  if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
    panning = true;
    panStart = mousePos;
    camStartTarget = camera.target;
  }
  if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
    panning = false;
  if (panning) {
    camera.target.x =
        camStartTarget.x - (mousePos.x - panStart.x) / camera.zoom;
    camera.target.y =
        camStartTarget.y - (mousePos.y - panStart.y) / camera.zoom;
  }

  if (mode == EditorMode::SELECT) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      selectedNode = -1;
      for (auto &[id, nd] : nodes) {
        if (Dist2(worldMouse, nd.pos) < NODE_R * NODE_R) {
          selectedNode = id;
          dragging = true;
          dragOffset = {worldMouse.x - nd.pos.x, worldMouse.y - nd.pos.y};
          break;
        }
      }
    }
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && dragging && selectedNode != -1)
      nodes[selectedNode].pos = {worldMouse.x - dragOffset.x,
                                 worldMouse.y - dragOffset.y};
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
      dragging = false;
  }

  if (mode == EditorMode::ADD_NODE && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    bool onNode = false;
    for (auto &[id, nd] : nodes)
      if (Dist2(worldMouse, nd.pos) < NODE_R * NODE_R * 4) {
        onNode = true;
        break;
      }
    if (!onNode) {
      AddNode(nextId, worldMouse);
      SpawnParticles(GetWorldToScreen2D(worldMouse, camera), C_ACCENT, 10);
      Notify("Node " + std::to_string(nextId - 1) + " eklendi", C_ACCENT);
    }
  }

  if (mode == EditorMode::ADD_EDGE) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      for (auto &[id, nd] : nodes) {
        if (Dist2(worldMouse, nd.pos) < NODE_R * NODE_R) {
          if (edgeSourceNode == -1) {
            edgeSourceNode = id;
            Notify("Source: node " + std::to_string(id) + " -> indi target sec",
                   C_PURPLE);
          } else if (edgeSourceNode != id) {
            float w = edgeWtIdx > 0 ? (float)atof(edgeWtBuf) : 1.0f;
            graph.AddEdge(edgeSourceNode, id, w);
            SpawnParticles(GetWorldToScreen2D(nd.pos, camera), C_PURPLE, 8);
            Notify("Edge " + std::to_string(edgeSourceNode) + " -> " +
                       std::to_string(id),
                   C_PURPLE);
            edgeSourceNode = -1;
          }
          break;
        }
      }
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
      edgeSourceNode = -1;
      Notify("Edge secimleri sifirlandir", C_MUTED);
    }
  }

  if (mode == EditorMode::DELETE && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    for (auto &[id, nd] : nodes) {
      if (Dist2(worldMouse, nd.pos) < NODE_R * NODE_R) {
        graph.RemoveNode(id);
        nodes.erase(id);
        Notify("Node " + std::to_string(id) + " silindi", C_ACCENT2);
        algoSteps.clear();
        currentStep = -1;
        break;
      }
    }
  }
}

void Editor::Draw() {
  int canvasW = GetScreenWidth() - PANEL_W;
  int canvasH = GetScreenHeight();

  for (int gx = -1; gx <= canvasW / 40 + 1; gx++) {
    for (int gy = -1; gy <= canvasH / 40 + 1; gy++) {
      float wx =
          PANEL_W + gx * 40 - fmodf(camera.target.x * camera.zoom, 40.0f);
      float wy = gy * 40 - fmodf(camera.target.y * camera.zoom, 40.0f);
      DrawPixel((int)wx, (int)wy, {30, 41, 59, 255});
    }
  }

  DrawEdges();

  if (mode == EditorMode::ADD_EDGE && edgeSourceNode != -1 &&
      nodes.count(edgeSourceNode)) {
    Vector2 src = GetWorldToScreen2D(nodes[edgeSourceNode].pos, camera);
    DrawLineEx(src, GetMousePosition(), 2.0f, WithAlpha(C_PURPLE, 160));
    DrawCircleV(GetMousePosition(), 5.0f, C_PURPLE);
  }

  for (auto &[id, nd] : nodes)
    DrawNode(id, const_cast<NodeData &>(nd));

  DrawParticles();
  DrawNotifications();

  const char *modeStr[] = {"SELECT", "ADD NODE", "ADD EDGE", "DELETE"};
  Color modeCol[] = {C_MUTED, C_ACCENT, C_PURPLE, C_ACCENT2};
  DrawRectangleRounded({(float)PANEL_W + 10, 10, 130, 26}, 0.4f, 8,
                       WithAlpha(modeCol[(int)mode], 200));
  int tw = MeasureText(modeStr[(int)mode], 13);
  DrawText(modeStr[(int)mode], PANEL_W + 10 + (130 - tw) / 2, 17, 13, C_PANEL);

  if (!algoSteps.empty() && currentStep >= 0) {
    auto &step = algoSteps[currentStep];
    if (!step.path.empty()) {
      std::string pathStr = "Path: ";
      for (int i = 0; i < (int)step.path.size(); i++) {
        if (i)
          pathStr += " -> ";
        pathStr += std::to_string(step.path[i]);
      }
      int ptw = MeasureText(pathStr.c_str(), 14);
      DrawRectangleRounded(
          {(float)PANEL_W + 10, (float)canvasH - 40, (float)ptw + 20, 28}, 0.4f,
          8, WithAlpha(C_PANEL, 220));
      DrawText(pathStr.c_str(), PANEL_W + 20, canvasH - 32, 14, C_GOLD);
    }
  }

  const char *zoomStr =
      TextFormat("%.0f%%  [R]=reset  [Del]=sil", camera.zoom * 100);
  DrawText(zoomStr, PANEL_W + canvasW - MeasureText(zoomStr, 12) - 12,
           canvasH - 22, 12, C_MUTED);

  if (nodes.empty()) {
    const char *hint = "ADD NODE modunu sec ve canvas-a tiqla";
    int hw = MeasureText(hint, 16);
    DrawText(hint, PANEL_W + (canvasW - hw) / 2, canvasH / 2, 16, C_MUTED);
  }
}
