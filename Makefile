CXX = g++
CXXFLAGS = -Iinclude -Wall -std=c++17
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = src/main.cpp src/Graph/Graph.cpp src/Editor/Editor.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)

bin/graph_editor: $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LIBS)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build/* bin/*
