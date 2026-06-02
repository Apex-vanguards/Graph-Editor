CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

SRCDIR   := src
INCDIR   := include
BUILDDIR := build

SRCS := $(SRCDIR)/main.cpp \
        $(SRCDIR)/Editor/Editor.cpp \
        $(SRCDIR)/Graph/Graph.cpp

# ─────────────────────────────────────────────
#  LINUX
# ─────────────────────────────────────────────
linux: TARGET   := bin/graph_editor
linux: INCLUDES := -I$(INCDIR) -I$(SRCDIR)
linux: LDFLAGS  := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
linux: OBJS     := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/linux/%.o, $(SRCS))
linux: $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/linux/%.o, $(SRCS))
	@mkdir -p bin
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)
	@echo ">>> Linux build done: ./$(TARGET)"

$(BUILDDIR)/linux/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ─────────────────────────────────────────────
#  WINDOWS  (cross-compile with MinGW)
#  toolchain: x86_64-w64-mingw32-g++
#  winlib/   holds libraylib.a + raylib.dll
# ─────────────────────────────────────────────
WINDIR   := win-lib
WINCXX   := x86_64-w64-mingw32-g++
WIN_INC  := -I$(INCDIR) -I$(SRCDIR) -I$(WINDIR)
WIN_LIBS := -L$(WINDIR) -lraylib -lopengl32 -lgdi32 -lwinmm
WIN_OBJ  := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/windows/%.o, $(SRCS))

windows: TARGET := graph_editor.exe
windows: $(WIN_OBJ)
	@mkdir -p bin
	$(WINCXX) $(CXXFLAGS) $^ -o bin/graph_editor.exe $(WIN_LIBS)
	@cp $(WINDIR)/raylib.dll bin/
	@echo ">>> Windows build done: bin/graph_editor.exe  (raylib.dll copied)"

$(BUILDDIR)/windows/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(WINCXX) $(CXXFLAGS) $(WIN_INC) -c $< -o $@

# ─────────────────────────────────────────────
#  CLEAN
# ─────────────────────────────────────────────
clean:
	rm -rf $(BUILDDIR) bin

.PHONY: linux windows clean
