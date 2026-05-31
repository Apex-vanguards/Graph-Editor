#include "../include/Editor/Editor.hpp"
#include "../include/Graph/Graph.hpp"
#include "raylib.h"

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(screenWidth, screenHeight, "Graph Editor");
  SetTargetFPS(60);

  Graph graph;
  Editor editor(graph);

  /*
  editor.AddNode(1, {-100, -100});
  editor.AddNode(2, {100, -100});
  editor.AddNode(3, {200, 50});
  editor.AddNode(4, {0, 150});
  editor.AddNode(5, {-200, 50});
  graph.AddEdge(1, 2, 4);
  graph.AddEdge(1, 5, 2);
  graph.AddEdge(2, 3, 5);
  graph.AddEdge(3, 4, 3);
  graph.AddEdge(4, 5, 1);
  graph.AddEdge(2, 4, 8);
  */

  while (!WindowShouldClose()) {
    editor.Update();
    editor.HandleUI();
    editor.HandleInput();

    BeginDrawing();
    ClearBackground({10, 12, 20, 255});

    editor.Draw();
    editor.DrawUI();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
