#include <cxxopts.hpp>

#include "config.h.in"
#include "constants.h"
#include "gridGenerator.h"

static auto run(const GInt debug, GInt argc, GChar** argv) -> GInt {
  if(debug == NO_DEBUG) {
    GRIDGEN::GridGenerator<NO_DEBUG> gridGen{};
    return gridGen.run(argc, argv);
  }
  if(debug == MIN_DEBUG) {
    GRIDGEN::GridGenerator<MIN_DEBUG> gridGen{};
    return gridGen.run(argc, argv);
  }
  if(debug == DEBUG) {
    GRIDGEN::GridGenerator<DEBUG> gridGen{};
    return gridGen.run(argc, argv);
  }
  if(debug == MORE_DEBUG) {
    GRIDGEN::GridGenerator<MORE_DEBUG> gridGen{};
    return gridGen.run(argc, argv);
  }
  GRIDGEN::GridGenerator<MAX_DEBUG> gridGen{};
  return gridGen.run(argc, argv);
}

auto main(int argc, GChar** argv) -> int {
  cxxopts::Options options("GridGenerator", "A highly parallel grid generator.");

  options.add_options()("d,debug", "Enable debugging with given level.", cxxopts::value<GInt>()->default_value("0"));
  options.add_options()("h,help", "Print usage");

  auto result = options.parse(argc, argv);

  if(result.count("help") > 0) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  GInt debug = result["debug"].as<GInt>();
  if(debug > 0) {
    if(debug > MAX_DEBUG) {
      debug = MAX_DEBUG;
    }
    std::cout << "Activated debug level " << DEBUG_LEVEL.at(debug) << std::endl;
  }

  return run(debug, argc, argv);
}
