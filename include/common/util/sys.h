#ifndef GRIDGENERATOR_SYS_H
#define GRIDGENERATOR_SYS_H

#include <array>
#include <date.h>
#include <dirent.h>
#include <filesystem>
#include <pwd.h>
#include <unistd.h>

#include "common/sfcmm_types.h"

/// Get the current time as YYYY-MM-DD HH:MM:SS
/// \return Returns the current time stamp as a string.
inline auto dateString() -> GString { return date::format("%F %X", std::chrono::system_clock::now()); }

/// Get the name of the host that the application is running on.
/// \return Returns the name of the host as a string.
inline auto hostString() -> GString {
  static constexpr GInt bufferSize = 1024;

  // Get current hostname
  std::array<char, bufferSize> host_{};
  gethostname(&host_[0], bufferSize - 1);
  host_[bufferSize - 1] = '\0';
  return static_cast<GString>(&host_[0]);
}

/// Get the current working directory.
/// \return Returns the current working directory as a string.
inline auto getCWD() -> GString { return std::filesystem::current_path(); }

/// Check if the given file name is already existing.
/// \param name File name to check for existence.
/// \return File exists?
inline auto isFile(const std::string& name) -> GBool { return std::filesystem::exists(name); }
#endif // GRIDGENERATOR_SYS_H
