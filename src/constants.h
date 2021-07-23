#ifndef GRIDGENERATOR_CONSTANTS_H
#define GRIDGENERATOR_CONSTANTS_H

#include <array>
#include <limits>
#include <vector>
#include "types.h"

static constexpr GFloat  GFloatEps  = std::numeric_limits<GFloat>::epsilon();
static constexpr GDouble GDoubleEps = std::numeric_limits<GDouble>::epsilon();

static constexpr GInt    DEFAULT_MAXNOOFFSPRINGS = 100000;
static constexpr GInt    BASE2                   = 2;
static constexpr GDouble HALF                    = 0.5;
static constexpr GInt    INVALID_CELLID          = -1;
static constexpr GInt    KBIT                    = 1024;
static constexpr GDouble DKBIT                   = 1024;

// arbitrary maximum cell level
static constexpr GInt MAX_LVL = 100;

enum class Debug_Level { no_debug, min_debug, debug, more_debug, max_debug };

static const std::array<GString, 5> DEBUG_LEVEL = {"NO DEBUG", "MINIMAL DEBUG", "DEBUG", "MORE DEBUG", "MAXIMUM DEBUG"};
static const std::vector<std::vector<GDouble>> DEFAULT_BOUNDINGBOX = {
    {0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0}};

// just some spaces to arrange output
static const GString SP1{"  "};
static const GString SP2{"    "};
static const GString SP3{"      "};
static const GString SP4{"        "};
static const GString SP5{"          "};
static const GString SP6{"            "};
static const GString SP7{"              "};


#endif
