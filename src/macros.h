#ifndef GRIDGENERATOR_MACROS_H
#define GRIDGENERATOR_MACROS_H

#include <iostream>
#include <mpi.h>
#include <sstream>
#include "backtrace.h"
#include "global.h"
#include "globalmpi.h"

/// Define macros to stringify literal and expanded macro arguments
///
/// STRINGIFY() can be used to stringify literal macro arguments (e.g.
/// STRINGIFY(__FILE__) becomes "__FILE__"), while XSTRINGIFY() will expand a
/// macro first (e.g. XSTRINGIFY(__FILE__) becomes "macros.h").
#define STRINGIFY(s) #s
#define XSTRINGIFY(s) STRINGIFY(s)

/// Define a short-hand macros for the location in the code (<file>:<line>)
#define LOC_ __FILE__ ":" XSTRINGIFY(__LINE__)

#define FUN_ __PRETTY_FUNCTION__

#define AT_ std::string(FUN_) + " (" + LOC_ + ")"

[[noreturn]] inline void term(const int errorCode, const std::string& location, const std::string& message = "") {
  if(errorCode != 0) {
    std::stringstream s;
    s << "\n";
    s << "Rank " << MPI::globalDomainId() << " threw exit code " << errorCode << "\n";
    s << "Error in " << location << ": " << message << "\n";
    s << "\n"
      << "Program is aborting!!\n";
    std::cerr << s.str() << std::flush;

    // Print backtrace (if enabled)
    BACKTRACE();

    // Close the log file to make sure that no MPI error occurs from the
    // unclosed file, and that a proper XML footer is written
    gridgen_log.close(true);
    MPI_Abort(MPI_COMM_WORLD, errorCode);
  } else {
    // memDealloc();

    // Close the log file to make sure that no MPI error occurs from the
    // unclosed file, and that a proper XML footer is written
    gridgen_log.close();
    // Call MPI_Finalize to ensure proper MPI shutdown
    MPI_Finalize();

    // Exit the program
    exit(0);
  }
  exit(errorCode);
}

#define TERMM(exitval, msg)                                                                                            \
  do {                                                                                                                 \
    term(exitval, AT_, msg);                                                                                           \
  } while(false)

#endif // GRIDGENERATOR_MACROS_H
