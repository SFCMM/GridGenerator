#ifndef GRIDGENERATOR_CONSTANTS_H
#define GRIDGENERATOR_CONSTANTS_H

#include <array>
#include <limits>
#include "types.h"

static constexpr GFloat GFloatEps = std::numeric_limits<GFloat>::epsilon();
static constexpr GDouble GDoubleEps = std::numeric_limits<GDouble>::epsilon();

enum class Debug_Level {no_debug, min_debug, debug, more_debug, max_debug};

static const std::array<GString, 5> DEBUG_LEVEL = {"NO DEBUG", "MINIMAL DEBUG", "DEBUG", "MORE DEBUG", "MAXIMUM DEBUG"};


#endif
