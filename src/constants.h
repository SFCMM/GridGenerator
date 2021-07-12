#ifndef GRIDGENERATOR_CONSTANTS_H
#define GRIDGENERATOR_CONSTANTS_H

static constexpr int NO_DEBUG   = 0;
static constexpr int MIN_DEBUG  = 1;
static constexpr int DEBUG      = 2;
static constexpr int MORE_DEBUG = 3;
static constexpr int MAX_DEBUG  = 4;

static const std::array<std::string, 5> DEBUG_LEVEL = {"NO DEBUG", "MINIMAL DEBUG", "DEBUG", "MORE DEBUG",
                                                       "MAXIMUM DEBUG"};


#endif
