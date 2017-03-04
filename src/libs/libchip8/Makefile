LIB = libchip8.a

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++11 -pipe

INCLUDE = -I"/usr/include/SDL" -I"../../../modules/cereal/include"

CXXFILES   = AudioDevice.cpp BitmappedGraphics.cpp Chip8.cpp ColourPalette.cpp Configuration.cpp ConfigurationReader.cpp Controller.cpp GameController.cpp GraphicsPlane.cpp KeyboardDevice.cpp Memory.cpp Schip.cpp XoChip.cpp

CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = stdafx.h.gch

all: opt

opt: CXXFLAGS += -DNDEBUG -march=native -O2
opt: $(LIB)

debug: CXXFLAGS += -g -DDEBUG
debug: $(LIB)

coverage: CXXFLAGS += -g -DDEBUG -fprofile-arcs -ftest-coverage
coverage: $(LIB)

$(PCH): stdafx.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -x c++-header $<

$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)

%.o: %.cpp $(PCH)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(LIB) $(OBJECTS) $(PCH)
