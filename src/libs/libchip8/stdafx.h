#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <cstdint>
#include <random>
#include <memory>
#include <functional>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <algorithm>

#include <iostream>
#include <fstream>

#include <array>
#include <vector>
#include <map>
#include <unordered_set>

#include <boost/format.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cereal/types/polymorphic.hpp>
#ifdef _DEBUG
#	include <cereal/archives/json.hpp>
#else
#	include <cereal/archives/binary.hpp>
#endif
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_set.hpp>

#include <SDL.h>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#endif
