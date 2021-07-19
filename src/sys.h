#ifndef GRIDGENERATOR_SYS_H
#define GRIDGENERATOR_SYS_H

#include <array>
#include <date.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

#include "macros.h"
#include "types.h"


inline auto dateString() -> GString{
  return date::format("%F %X", std::chrono::system_clock::now());
}

inline auto hostString() -> GString {
  static constexpr GInt bufferSize = 1024;

  // Get current hostname
  std::array<char, bufferSize> host_{};
  gethostname(&host_[0], bufferSize - 1);
  host_[bufferSize - 1] = '\0';
  return static_cast<GString> (&host_[0]);
}

inline auto getCWD() -> GString {
  static constexpr GInt bufferSize = 1024;

  // Get current directory
  std::array<char, bufferSize> dir_{};
  if(getcwd(&dir_[0], bufferSize - 1) == nullptr) {
    TERMM(-1, "No valid directory!");
  }
  dir_[bufferSize - 1] = '\0';
  return static_cast<GString> (&dir_[0]);
}

inline auto isFile (const std::string& name) -> GBool {
  struct stat buffer{};
  return (stat (name.c_str(), &buffer) == 0);
}
#endif // GRIDGENERATOR_SYS_H
