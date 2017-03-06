EXE = testchip8

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++11 -pipe
LDFLAGS  = -L../libs/libchip8 -lchip8 `sdl2-config --libs` -lboost_program_options

INCLUDE = -I"/usr/include/SDL" -I../libs/libchip8 -I../../modules/catch/single_include -I../../modules/cereal/include

CXXFILES   = testchip8.cpp chip8_tests.cpp schip_tests.cpp
CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = stdafx.h.gch

all: opt

opt: CXXFLAGS += -DNDEBUG -march=native -O2
opt: LDFLAGS += -s
opt: $(EXE)

debug: CXXFLAGS += -g -D_DEBUG
debug: LDFLAGS += -g
debug: $(EXE)

coverage: CXXFLAGS += -g -D_DEBUG -fprofile-arcs -ftest-coverage
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
