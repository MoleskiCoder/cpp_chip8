#ifdef _MSC_VER
#pragma once
#endif

#include <array>
#include <vector>
#include <unordered_set>

#include <string>

#include <memory>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <random>
#include <limits>
#include <stdexcept>
#include <cmath>

#include <SDL.h>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cereal/types/polymorphic.hpp>
#ifdef _DEBUG
#	include <cereal/archives/json.hpp>
#else
#	include <cereal/archives/binary.hpp>
#endif
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_set.hpp>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#endif
