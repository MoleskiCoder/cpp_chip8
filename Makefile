EXE = cpp_chip8

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++11 -march=native -O2 -pipe
LDFLAGS  = `sdl2-config --libs` -lboost_program_options -s

INCLUDE = -I"/usr/include/SDL" -I"modules/cereal/include"

CXXFILES   = $(shell find src -maxdepth 1 -type f -name '*.cpp')
CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = src/stdafx.h.gch

all: $(EXE)

$(PCH): src/stdafx.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -x c++-header $<

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)

src/%.o: src/%.cpp $(PCH)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(EXE) $(OBJECTS) $(PCH)
