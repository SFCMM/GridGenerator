#ifndef GRIDGENERATOR_CONSTANTS_H
#define GRIDGENERATOR_CONSTANTS_H

#include <array>
#include <limits>
#include "types.h"

static constexpr GDouble GDoubleEps = std::numeric_limits<GDouble>::epsilon();

static constexpr GInt NO_DEBUG   = 0;
static constexpr GInt MIN_DEBUG  = 1;
static constexpr GInt DEBUG      = 2;
static constexpr GInt MORE_DEBUG = 3;
static constexpr GInt MAX_DEBUG  = 4;

static const std::array<GString, 5> DEBUG_LEVEL = {"NO DEBUG", "MINIMAL DEBUG", "DEBUG", "MORE DEBUG", "MAXIMUM DEBUG"};


#endif
