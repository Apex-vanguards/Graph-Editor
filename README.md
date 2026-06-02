<div align="center">

```
 ██████╗ ██████╗  █████╗ ██████╗ ██╗  ██╗    ███████╗██████╗ ██╗████████╗ ██████╗ ██████╗ 
██╔════╝ ██╔══██╗██╔══██╗██╔══██╗██║  ██║    ██╔════╝██╔══██╗██║╚══██╔══╝██╔═══██╗██╔══██╗
██║  ███╗██████╔╝███████║██████╔╝███████║    █████╗  ██║  ██║██║   ██║   ██║   ██║██████╔╝
██║   ██║██╔══██╗██╔══██║██╔═══╝ ██╔══██║    ██╔══╝  ██║  ██║██║   ██║   ██║   ██║██╔══██╗
╚██████╔╝██║  ██║██║  ██║██║     ██║  ██║    ███████╗██████╔╝██║   ██║   ╚██████╔╝██║  ██║
 ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝    ╚══════╝╚═════╝ ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝
```

**An interactive graph algorithm visualizer built with C++17 and raylib**

![C++](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B)
![raylib](https://img.shields.io/badge/raylib-5.x-white?style=flat-square)
![Linux](https://img.shields.io/badge/Linux-supported-FCC624?style=flat-square&logo=linux&logoColor=black)
![Windows](https://img.shields.io/badge/Windows-supported-0078D6?style=flat-square&logo=windows)
![License](https://img.shields.io/badge/license-GPL-blue)

</div>

---

## What is this?

Graph Editor is a real-time, interactive graph algorithm visualizer. You can build any graph by hand — add nodes, draw edges, set weights — then watch BFS, DFS, Dijkstra, or Bellman-Ford animate step by step directly on the canvas. Every visited node lights up, every traversed edge glows, and the shortest path is highlighted in gold when the algorithm finishes.

Built entirely in C++17 with [raylib](https://www.raylib.com/) for rendering. No external dependencies beyond raylib itself.

---

## Features

### Editor

| Feature | Description |
|---|---|
| **4 editor modes** | Select & drag, Add node, Add edge, Delete |
| **Directed / Undirected** | Toggle at any time; arrow heads drawn for directed graphs |
| **Weighted edges** | Set custom float weights per edge; displayed inline on the canvas |
| **Camera controls** | Middle-mouse pan, scroll-wheel zoom, `R` to reset view |
| **Click-to-connect** | In Add Edge mode, click source then target — live preview line drawn |
| **Keyboard shortcuts** | `Esc` to cancel, `Del` to delete selected node, `R` to reset camera |

### Algorithms

| Algorithm | Type | Highlights |
|---|---|---|
| **BFS** | Breadth-First Search | Level-by-level traversal, blue visited nodes |
| **DFS** | Depth-First Search | Stack-based deep traversal, purple visited nodes |
| **Dijkstra** | Shortest Path | Greedy priority queue, gold path on completion |
| **Bellman-Ford** | Shortest Path | Edge relaxation, works with negative weights |

### Visualization

- Nodes pulse and glow when visited or on the shortest path
- Active edges light up yellow as the algorithm processes them
- Shortest path edges highlighted in gold
- Full path printed at the bottom of the canvas: `Path: 1 → 3 → 4 → 2`
- Particle burst animations on node/edge creation
- Toast notifications for every action (top-right corner)
- Step-by-step playback with Prev / Play / Next controls
- Adjustable animation speed (Faster / Slower)
- Progress bar showing current step out of total

---

## Screenshots

```
┌──────────────────────────────────────────────────────────────────┐
│  GRAPH EDITOR v2.1          [SELECT] [ADD NODE] [ADD EDGE] [DEL] │
│ ─────────────────           ──────────────────────────────────── │
│  GRAPH SETTINGS             ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · │
│  [Directed] [Undirected]    ·  ·  ◉①──────────◉②  ·  ·  ·  ·  · │
│ ─────────────────           ·  ·   \    4     / \  ·  ·  ·  ·  · │
│  ADD EDGE                   ·  · 2  \        /5  \ ·  ·  ·  ·  · │
│  From  To  Weight           ·  ·    ◉⑤      ◉③  8\ ·  ·  ·  ·  · │
│  [1 ] [3 ] [2.0]            ·  ·     \  1  /    / ·  ·  ·  ·  · │
│  [+ ADD EDGE      ]         ·  ·      ◉④──────── ·  ·  ·  ·  · │
│  [Clear Edges][Clear All]   ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · │
│ ─────────────────           ──────────────────────────────────── │
│  ALGORITHM                  Path: 1 → 5 → 4 → 3          100%   │
│  [BFS ] [DFS ]              └─────────────────────────────────── │
│  [Dijkstra][Bellman-Ford]                                         │
│  Start Node  End Node                                             │
│  [1        ] [3        ]                                          │
│  [▶  RUN ALGORITHM    ]                                           │
│ ─────────────────                                                 │
│  PLAYBACK                                                         │
│  ████████████░░░░░░░░  Step 5 / 8                                 │
│  [◀ Prev][▶ Play][Next▶]                                          │
│  Speed ████████░░░░░░░                                            │
│  [Faster ▲]  [Slower ▼]                                           │
│  [Reset           ]                                               │
└──────────────────────────────────────────────────────────────────┘
```

---

## Project Structure

```
graph_editor/
├── bin/                        # Compiled binaries (generated)
│   ├── graph_editor            # Linux executable
│   ├── graph_editor.exe        # Windows executable
│   └── raylib.dll              # Windows runtime DLL
├── build/                      # Object files (generated)
│   ├── linux/
│   └── windows/
├── include/
│   ├── Editor/
│   │   └── Editor.hpp          # Editor class — UI, input, rendering
│   ├── Graph/
│   │   └── Graph.hpp           # Graph class — adjacency list, algorithm results
│   └── raylib.h
├── src/
│   ├── Editor/
│   │   └── Editor.cpp
│   ├── Graph/
│   │   └── Graph.cpp
│   ├── main.cpp
│   ├── raylib.h
│   └── raymath.h
├── win-lib/                    # Windows raylib binaries
│   ├── libraylib.a
│   ├── libraylibdll.a
│   ├── raylib.dll
│   ├── raylib.h
│   └── raymath.h
└── Makefile
```

---

## Building

### Requirements

**Linux**

```bash
# Ubuntu / Debian
sudo apt install build-essential libraylib-dev

# Arch
sudo pacman -S raylib
```

**Windows cross-compile** (from Linux)

```bash
sudo apt install mingw-w64
```

The `win-lib/` folder must contain `libraylib.a` and `raylib.dll` built for Windows x86_64. Download them from [github.com/raysan5/raylib/releases](https://github.com/raysan5/raylib/releases).

---

### Build Commands

```bash
# Linux binary → bin/graph_editor
make linux

# Windows binary → bin/graph_editor.exe + bin/raylib.dll
make windows

# Remove all build artifacts and binaries
make clean
```

---

## Running

**Linux**

```bash
./bin/graph_editor
```

**Windows**

```
bin\graph_editor.exe
```

`raylib.dll` must be in the same folder as the `.exe` — the build system copies it there automatically.

---

## How to Use

### Building a Graph

1. **Add nodes** — Switch to `ADD NODE` mode, click anywhere on the canvas
2. **Add edges (canvas)** — Switch to `ADD EDGE` mode, click the source node, then the target node
3. **Add edges (panel)** — Type `From`, `To`, and `Weight` in the left panel, press `+ ADD EDGE`
4. **Move nodes** — `SELECT` mode, click and drag any node
5. **Delete** — `DELETE` mode and click a node, or select a node and press `Del`

### Running an Algorithm

1. Choose **BFS** or **DFS** (only Start Node needed), or **Dijkstra** / **Bellman-Ford** (Start + End)
2. Type the start node ID in the `Start Node` field
3. For path algorithms, type the end node ID in `End Node`
4. Press **▶ RUN ALGORITHM**
5. Use **Prev / Play / Next** to step through the animation
6. Adjust speed with **Faster ▲ / Slower ▼**
7. Press **Reset** to clear the visualization and try again

### Camera

| Action | Control |
|---|---|
| Pan | Middle mouse button + drag |
| Zoom in / out | Scroll wheel |
| Reset view | `R` |

---

## Algorithm Details

### BFS — Breadth-First Search

Explores all neighbors at the current depth before going deeper. Uses a queue. Guarantees shortest path in **unweighted** graphs.

### DFS — Depth-First Search

Explores as far as possible along each branch before backtracking. Uses a stack. Useful for cycle detection, topological sort, connectivity checks.

### Dijkstra

Finds the shortest path between two nodes in a graph with **non-negative** edge weights. Uses a min-heap priority queue. Time complexity: **O((V + E) log V)**.

### Bellman-Ford

Finds shortest paths from a source node by relaxing all edges repeatedly. Handles **negative edge weights**. Detects negative cycles. Time complexity: **O(V × E)**.

---

## Architecture

The project is split into two independent modules:

**`Graph`** — Pure data structure. Holds the adjacency list and implements all four algorithms. Each algorithm returns a `vector<StepResult>` where every step records which nodes have been visited, which edges are active, and what the current shortest path is. The Graph class has zero rendering code.

**`Editor`** — Owns the Graph by reference. Manages the raylib window loop, all UI drawing, input handling, camera, particles, and notifications. Calls Graph algorithms and plays back the returned steps frame by frame.

This separation means you can swap out the renderer or plug the Graph into a different frontend without touching the algorithm logic.

---

## License

GNU — do whatever you want with it.

---

<div align="center">
Built with C++17 · raylib · a lot of graph theory
</div>
