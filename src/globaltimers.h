#ifndef GRIDGENERATOR_GLOBALTIMERS_H
#define GRIDGENERATOR_GLOBALTIMERS_H

namespace gridgenerator {
// Create struct for easy timer identification
struct Timers_ {
  // Enum to store timer "names"
  enum {
    //Timer groups
    AppGroup,


    timertotal,
    IO,
    Init,

    GridGeneration,
    GridUniform,
    GridRefinement,

    // counter
    _count
  };
};
} // namespace gridgenerator

#endif // GRIDGENERATOR_GLOBALTIMERS_H
