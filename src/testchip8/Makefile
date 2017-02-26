EXE = testchip8

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++11 -march=native -O2 -pipe
LDFLAGS  = -s -L../libs/libchip8 -lchip8 `sdl2-config --libs` -lboost_program_options

INCLUDE = -I"/usr/include/SDL" -I../libs/libchip8 -I../../modules/catch/single_include/

CXXFILES   = testchip8.cpp chip8_tests.cpp
CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = stdafx.h.gch

all: $(EXE)

$(PCH): stdafx.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -x c++-header $<

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)

%.o: %.cpp $(PCH)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(EXE) $(OBJECTS) $(PCH)