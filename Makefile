EXE = cpp_chip8

CXXFLAGS = -Wall -g `sdl2-config --cflags` -std=c++11 -march=native -O2 -pipe
LDFLAGS  = `sdl2-config --libs` -lboost_program_options

INCLUDE = -I"/usr/include/SDL"

CXXFILES   = $(shell find src -maxdepth 1 -type f -name '*.cpp')
CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(EXE) $(OBJECTS)
