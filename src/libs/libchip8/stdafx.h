#ifdef _MSC_VER
#pragma once
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

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
