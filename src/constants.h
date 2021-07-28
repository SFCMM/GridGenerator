#ifndef GRIDGENERATOR_CONSTANTS_H
#define GRIDGENERATOR_CONSTANTS_H

#include <array>
#include <limits>
#include <vector>
#include "functions.h"
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
static constexpr GInt MAX_DIM = 4;

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

/// Return maximum number of children per cell
template <GInt NDIM>
static constexpr inline auto maxNoChildren() -> GInt {
  return gcem::pow(2, NDIM);
}

/// Return maximum number of Neighbors per cell
template <GInt NDIM>
static constexpr inline auto maxNoNghbrs() -> GInt {
  return 2 * NDIM;
}


// todo: replace with constant expression function
static constexpr std::array<std::array<GDouble, MAX_DIM>, maxNoChildren<MAX_DIM>()> childDir = {{
    //-> 2D
    // -x,-y, -z
    {{-1, -1, -1, -1}}, // 0
    //+x, -y, -z
    {{1, -1, -1, -1}}, // 1
    //-x, +y, -z
    {{-1, 1, -1, -1}}, // 2
    //+x, -y, -z
    {{1, 1, -1, -1}}, // 3
    //<- 2D

    //-> 3D (+z)
    {{-1, -1, 1, -1}}, // 4
    {{1, -1, 1, -1}},  // 5
    {{-1, 1, 1, -1}},  // 6
    {{1, 1, 1, -1}},   // 7
    //<- 3D (+z)

    //-> 4D
    {{-1, -1, -1, 1}}, // 8
    {{1, -1, -1, 1}},  // 9
    {{-1, 1, -1, 1}},  // 10
    {{1, 1, -1, 1}},   // 11
    {{-1, -1, 1, 1}},  // 12
    {{1, -1, 1, 1}},   // 13
    {{-1, 1, 1, 1}},   // 14
    {{1, 1, 1, 1}}     // 15
    //<- 4D
}};

// todo: replace with constant expression function
static constexpr std::array<std::array<GDouble, maxNoNghbrs<MAX_DIM>()>, maxNoChildren<MAX_DIM>()> nghbrInside = {{
    //-x +x -y +y -z +z -zz +zz
    {{-1, 1, -1, 2, -1, 4, -1, 8}},  // 0
    {{0, -1, -1, 3, -1, 5, -1, 9}},  // 1
    {{-1, 3, 0, -1, -1, 6, -1, 10}}, // 2
    {{2, -1, 1, -1, -1, 7, -1, 11}}, // 3
    {{-1, 5, -1, 6, 0, -1, -1, 12}}, // 4
    {{4, -1, -1, 7, 1, -1, -1, 13}}, // 5
    {{-1, 7, 4, -1, 2, -1, -1, 14}}, // 6
    {{6, -1, 5, -1, 3, -1, -1, 15}}, // 7


    // upper table +8 and last dir = -1
    {{-1, 9, -1, 10, -1, 12, 0, -1}},  // 8
    {{8, -1, -1, 11, -1, 13, 1, -1}},  // 9
    {{-1, 11, 8, -1, -1, 14, 2, -1}},  // 10
    {{10, -1, 9, -1, -1, 15, 3, -1}},  // 11
    {{-1, 13, -1, 14, 8, -1, 4, -1}},  // 12
    {{12, -1, -1, 15, 9, -1, 5, -1}},  // 13
    {{-1, 15, 12, -1, 10, -1, 6, -1}}, // 14
    {{14, -1, 13, -1, 11, -1, 7, -1}}  // 15
}};

// todo: replace with constant expression function
static constexpr std::array<std::array<GDouble, maxNoNghbrs<MAX_DIM>()>, maxNoChildren<MAX_DIM>()> nghbrParentChildId =
    {{
        //-x +x -y +y -z +z -zz +zz
        {{1, -1, 2, -1, 4, -1, 8, -1}},  // 0
        {{-1, 0, 3, -1, 5, -1, 9, -1}},  // 1
        {{3, -1, -1, 0, 6, -1, 10, -1}}, // 2
        {{-1, 2, -1, 1, 7, -1, 11, -1}}, // 3
        {{5, -1, 6, -1, -1, 0, 12, -1}}, // 4
        {{-1, 4, 7, -1, -1, 1, 13, -1}}, // 5
        {{7, -1, -1, 4, -1, 2, 14, -1}}, // 6
        {{-1, 6, -1, 5, -1, 3, 15, -1}}, // 7


        // upper table +8 and last dir = -1
        {{9, -1, 10, -1, 12, -1, -1, 0}},  // 8
        {{-1, 8, 11, -1, 13, -1, -1, 1}},  // 9
        {{11, -1, -1, 8, 14, -1, -1, 2}},  // 10
        {{-1, 10, -1, 9, 15, -1, -1, 3}},  // 11
        {{13, -1, 14, -1, -1, 8, -1, 4}},  // 12
        {{-1, 12, 15, -1, -1, 9, -1, 5}},  // 13
        {{15, -1, -1, 12, -1, 10, -1, 6}}, // 14
        {{-1, 14, -1, 13, -1, 11, -1, 7}}  // 15
    }};


#endif
