#include <cxxopts.hpp>

#include "constants.h"
#include "gridGenerator.h"

#ifdef _OPENMP
#include <omp.h>
#endif

auto main(int argc, char** argv) -> int {
  cxxopts::Options options("GridGenerator", "A highly parallel grid generator.");

  options.add_options()("d,debug", "Enable debugging with given level.", cxxopts::value<int>()->default_value("0"));
  options.add_options()("h,help", "Print usage");

  auto result = options.parse(argc, argv);

  if(result.count("help") > 0) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  int debug = result["debug"].as<int>();
  if(debug > 0) {
    if(debug > MAX_DEBUG) {
      debug = MAX_DEBUG;
    }
    std::cout << "Activated debug level " << DEBUG_LEVEL.at(debug) << std::endl;
  }

  int return_code = 0;
  if(debug == NO_DEBUG) {
    GridGenerator<NO_DEBUG> gridGen{};
    return_code = gridGen.run();
  } else if(debug == MIN_DEBUG) {
    GridGenerator<MIN_DEBUG> gridGen{};
    return_code = gridGen.run();
  } else if(debug == DEBUG) {
    GridGenerator<DEBUG> gridGen{};
    return_code = gridGen.run();
  } else if(debug == MORE_DEBUG) {
    GridGenerator<MORE_DEBUG> gridGen{};
    return_code = gridGen.run();
  } else if(debug == MAX_DEBUG) {
    GridGenerator<MAX_DEBUG> gridGen{};
    return_code = gridGen.run();
  }

  return return_code;
}