#ifndef GRIDGENERATOR_SYS_H
#define GRIDGENERATOR_SYS_H

#include <array>
#include <date.h>
#include <dirent.h>
#include <filesystem>
#include <pwd.h>
#include <unistd.h>

#include "macros.h"
#include "types.h"


inline auto dateString() -> GString { return date::format("%F %X", std::chrono::system_clock::now()); }

inline auto hostString() -> GString {
  static constexpr GInt bufferSize = 1024;

  // Get current hostname
  std::array<char, bufferSize> host_{};
  gethostname(&host_[0], bufferSize - 1);
  host_[bufferSize - 1] = '\0';
  return static_cast<GString>(&host_[0]);
}

inline auto getCWD() -> GString { return std::filesystem::current_path(); }

inline auto isFile(const std::string& name) -> GBool { return std::filesystem::exists(name); }
#endif // GRIDGENERATOR_SYS_H
