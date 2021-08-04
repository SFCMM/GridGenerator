#ifndef GRIDGENERATOR_CONSTANTS_H
#define GRIDGENERATOR_CONSTANTS_H

#include <array>
#include <limits>
#include <vector>
#include "common/types.h"
#include "config.h"
#include "functions.h"

static constexpr GFloat  GFloatEps      = std::numeric_limits<GFloat>::epsilon();
static constexpr GDouble GDoubleEps     = std::numeric_limits<GDouble>::epsilon();
static constexpr GInt    BASE2          = 2;
static constexpr GDouble HALF           = 0.5;
static constexpr GInt    INVALID_CELLID = -1;
static constexpr GInt    KBIT           = 1024;
static constexpr GDouble DKBIT          = static_cast<GDouble>(KBIT);

enum class Debug_Level { no_debug, min_debug, debug, more_debug, max_debug };

static constexpr std::array<std::string_view, 5> DEBUG_LEVEL = {"NO DEBUG", "MINIMAL DEBUG", "DEBUG", "MORE DEBUG", "MAXIMUM DEBUG"};
static const std::vector<std::vector<GDouble>>   DEFAULT_BOUNDINGBOX = {
    {0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0}};

// just some spaces to arrange output
static constexpr std::string_view SP1{"  "};
static constexpr std::string_view SP2{"    "};
static constexpr std::string_view SP3{"      "};
static constexpr std::string_view SP4{"        "};
static constexpr std::string_view SP5{"          "};
static constexpr std::string_view SP6{"            "};
static constexpr std::string_view SP7{"              "};

namespace coordinate {
static constexpr std::array<char, 4> name = {'x', 'y', 'z', 'w'};
}
#endif
