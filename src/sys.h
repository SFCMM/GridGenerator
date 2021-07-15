#ifndef GRIDGENERATOR_SYS_H
#define GRIDGENERATOR_SYS_H

#include <date.h>

#include "types.h"


inline auto dateString() -> GString{
  return date::format("%F %X", std::chrono::system_clock::now());
}

#endif // GRIDGENERATOR_SYS_H
