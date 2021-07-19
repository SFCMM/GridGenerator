#include <cxxopts.hpp>

#include "config.h"
#include "constants.h"
#include "gridGenerator.h"
#include "sys.h"

class AppConfiguration{
 public:

  auto run(GInt debug) -> int{
    if(debug == NO_DEBUG) {
      return run<NO_DEBUG>();
    }
    if(debug == MIN_DEBUG) {
      return run<MIN_DEBUG>();
    }
    if(debug == DEBUG) {
      return run<DEBUG>();
    }
    if(debug == MORE_DEBUG) {
      return run<MORE_DEBUG>();
    }
    return run<MAX_DEBUG>();
  }

  template<int DEBUG>
  auto run() -> int{
    GRIDGEN::GridGenerator<DEBUG> gridGen{};
    gridGen.init(m_argc, m_argv, m_configurationFile);
    return gridGen.run();
  }

  void setCMD(int argc, GChar** argv){
    m_argc = argc;
    m_argv = argv;
  }

  void setConfigurationFile(GString& configFile){
    m_configurationFile = configFile;
  }

 private:
  GChar** m_argv{};
  int m_argc{};

  GString m_configurationFile = "grid.json";
};

auto main(int argc, GChar** argv) -> int {
  std::ostringstream tmpBuffer;
  tmpBuffer << "GridGenerator v"<< XSTRINGIFY(PROJECT_VER);
  cxxopts::Options options(tmpBuffer.str(), "A highly parallel grid generator.");

  options.add_options()("d,debug", "Enable debugging with given level.", cxxopts::value<GInt>()->default_value("0"));
  options.add_options()("h,help", "Print usage");
  options.add_options()("version", "Get version information");
  options.add_options()("c,config", "Configuration file (default=grid.json)", cxxopts::value<std::string>()
      ->default_value("grid.json"));

  options.parse_positional({"config"});
  auto result = options.parse(argc, argv);

  if(result.count("help") > 0) {
    std::cout << options.help() << std::endl;
    exit(0);
  }
  if(result.count("version") > 0) {
    std::cout << XSTRINGIFY(PROJECT_VER) <<" build " << XSTRINGIFY(BUILD_NUM) << std::endl;
    exit(0);
  }

  AppConfiguration gridGenRunner{};
  gridGenRunner.setCMD(argc, argv);

  GInt debug = result["debug"].as<GInt>();
  if(debug > 0) {
    if(debug > MAX_DEBUG) {
      debug = MAX_DEBUG;
    }
    std::cout << "Activated debug level " << DEBUG_LEVEL.at(debug) << std::endl;
  }

  //first positional argument should be the configuration file
  GString config_file = result["config"].as<GString>();
  //check if the file actually exists
  if(!isFile(config_file)){
    TERMM(-1, "Configuration file not found: "+ config_file);
  }


  return gridGenRunner.run(debug);
}
