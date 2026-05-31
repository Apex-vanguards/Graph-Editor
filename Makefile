CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS  := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRCDIR   := src
INCDIR   := include
BUILDDIR := build
TARGET   := bin/graph_editor

SRCS := $(SRCDIR)/main.cpp \
        $(SRCDIR)/Editor/Editor.cpp \
        $(SRCDIR)/Graph/Graph.cpp

OBJS := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: all clean
