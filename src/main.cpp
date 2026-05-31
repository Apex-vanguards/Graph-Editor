#include "../include/Editor/Editor.hpp"
#include "../include/Graph/Graph.hpp"
#include "raylib.h"

int main(int argc, char *argv[]) {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, "Graph Editor - Testing");
  SetTargetFPS(60);

  Graph graph;
  Editor editor(graph);

  editor.AddNode(1, {500, 200});
  editor.AddNode(2, {700, 300});
  editor.AddNode(3, {900, 200});

  while (!WindowShouldClose()) {

    editor.Update();
    editor.HandleUI();
    editor.HandleInput();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    editor.Draw();

    editor.DrawUI();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
