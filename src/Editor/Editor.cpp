#include "../../include/Editor/Editor.hpp"

void Editor::DrawUI() {
  DrawRectangle(0, 0, 300, 720, DARKGRAY);

  const char *algoNames[] = {"DFS", "BFS", "Dijkstra", "A*", "Bellman-Ford"};
  for (int i = 0; i < 5; i++) {
    DrawRectangle(20, 50 + (i * 40), 20, 20, WHITE);
    if (currentAlgo == (i + 1))
      DrawText("X", 25, 52 + (i * 40), 20, RED);
    DrawText(algoNames[i], 50, 50 + (i * 40), 20, WHITE);
  }

  DrawText("Start Node:", 20, 250, 20, WHITE);
  DrawRectangle(150, 245, 100, 30, WHITE);
  if (focusedInput == START_NODE_INPUT)
    DrawRectangleLines(150, 245, 100, 30, RED);
  DrawText(startNodeBuffer, 155, 250, 20, BLACK);

  DrawRectangle(20, 300, 260, 40, GREEN);
  DrawText("START ALGORITHM", 60, 310, 20, BLACK);

  DrawText("Node Count:", 20, 370, 20, WHITE);
  DrawRectangle(150, 365, 100, 30, WHITE);
  if (focusedInput == NODE_COUNT_INPUT)
    DrawRectangleLines(150, 365, 100, 30, RED);
  DrawText(nodeCountBuffer, 155, 370, 20, BLACK);

  DrawText("Add Edge:", 20, 420, 20, WHITE);
  DrawText("(u, v, w)", 20, 445, 15, LIGHTGRAY);
  DrawRectangle(20, 470, 260, 40, WHITE);
  if (focusedInput == EDGE_INPUT)
    DrawRectangleLines(20, 470, 260, 40, RED);
  DrawText(edgeBuffer, 25, 480, 20, BLACK);
}

void Editor::HandleUI() {
  Vector2 mouse = GetMousePosition();
  for (int i = 0; i < 5; i++) {
    if (CheckCollisionPointRec(mouse, {20, 50.0f + (i * 40), 20, 20}) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      currentAlgo = (AlgorithmType)(i + 1);
  }
  if (CheckCollisionPointRec(mouse, {150, 245, 100, 30}) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = START_NODE_INPUT;
  else if (CheckCollisionPointRec(mouse, {150, 365, 100, 30}) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = NODE_COUNT_INPUT;
  else if (CheckCollisionPointRec(mouse, {20, 470, 260, 40}) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = EDGE_INPUT;
  else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    focusedInput = NONE_FOCUSED;
}

void Editor::HandleInput() {
  int key = GetCharPressed();
  if (focusedInput == START_NODE_INPUT) {
    if (key >= 48 && key <= 57 && startNodeIndex < 9) {
      startNodeBuffer[startNodeIndex++] = (char)key;
      startNodeBuffer[startNodeIndex] = '\0';
    }
    if (IsKeyPressed(KEY_BACKSPACE) && startNodeIndex > 0)
      startNodeBuffer[--startNodeIndex] = '\0';
  } else if (focusedInput == NODE_COUNT_INPUT) {
    if (key >= 48 && key <= 57 && nodeCountIndex < 9) {
      nodeCountBuffer[nodeCountIndex++] = (char)key;
      nodeCountBuffer[nodeCountIndex] = '\0';
    }
    if (IsKeyPressed(KEY_BACKSPACE) && nodeCountIndex > 0)
      nodeCountBuffer[--nodeCountIndex] = '\0';
  } else if (focusedInput == EDGE_INPUT) {
    if (((key >= 48 && key <= 57) || key == 44 || key == 32) &&
        edgeIndex < 19) {
      edgeBuffer[edgeIndex++] = (char)key;
      edgeBuffer[edgeIndex] = '\0';
    }
    if (IsKeyPressed(KEY_BACKSPACE) && edgeIndex > 0)
      edgeBuffer[--edgeIndex] = '\0';
  }
}

void Editor::Update() {
  Vector2 mousePos = GetMousePosition();
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    for (auto &[id, node] : nodes) {
      if (CheckCollisionPointCircle(mousePos, node.pos, 20.0f)) {
        selectedNode = id;
        break;
      }
    }
  }
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selectedNode != -1)
    nodes[selectedNode].pos = mousePos;
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    selectedNode = -1;
}

void Editor::Draw() {
  for (auto &[id, node] : nodes) {
    DrawCircleV(node.pos, 20.0f, BLUE);
    DrawText(TextFormat("%d", id), node.pos.x - 5, node.pos.y - 5, 20, WHITE);
  }
}

void Editor::AddNode(int id, Vector2 position) { nodes[id] = {position}; }
