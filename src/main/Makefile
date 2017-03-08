EXE = chip8

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++11 -pipe
LDFLAGS  = `sdl2-config --libs` -lboost_program_options -L../libs/libchip8 -lchip8

INCLUDE = -I../libs/libchip8

CXXFILES   = main.cpp
CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = stdafx.h.gch

all: opt

opt: CXXFLAGS += -DNDEBUG -march=native -O2
opt: LDFLAGS += -s
opt: $(EXE)

debug: CXXFLAGS += -g -DDEBUG
debug: LDFLAGS += -g
debug: $(EXE)

coverage: CXXFLAGS += -g -DDEBUG -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -g -lgcov
coverage: $(EXE)

$(PCH): stdafx.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -x c++-header $<

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)

%.o: %.cpp $(PCH)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(EXE) $(OBJECTS) $(PCH) *.gcov *.gcda *.gcno
