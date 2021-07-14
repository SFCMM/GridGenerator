#ifndef GRIDGENERATOR_GLOBALMPI_H
#define GRIDGENERATOR_GLOBALMPI_H

#include <iostream>
#include "mpi.h"

namespace MPI {
/// Print all information of given MPI_Info object
void printMpiInfo(MPI_Info& mpiInfo);

////////////////////////////////////////////////////////////////////////////////
// Accessors and storage for global MPI information

/// Class to store global MPI information and to prevent accidental changes
class Information {
 public:
  void init(const int domainId, const int noDomains) {
    m_globalDomainId  = domainId;
    m_globalNoDomains = noDomains;

    initMPIInformation();
  }

 private:
  void initMPIInformation() {
    MPI_Info_create(&m_mpiInfo);

    // Set header align size to 10KB for netCDF files. Allows to append header data without the need
    // to move all variable data if the header size is exceeded (which may cause MPI I/O errors).
    // Source: https://trac.mcs.anl.gov/projects/parallel-netcdf/wiki/VariableAlignment
    MPI_Info_set(m_mpiInfo, "nc_header_align_size", "10240");
    // Note: possibility to set variable align size
    /* MPI_Info_set(m_mpiInfo, "nc_var_align_size", "4194304"); */


#ifdef MPI_IO_PRINT_INFO
    // Print MPI information on global rank 0
    if(m_globalDomainId == 0) {
      cerr0 << std::endl << "Global MPI information" << std::endl;
      printMpiInfo(m_mpiInfo);
    }
#endif
  }

  friend auto globalDomainId() -> int;
  friend auto globalNoDomains() -> int;
  friend auto globalMpiInfo() -> const MPI_Info&;

  int      m_globalDomainId  = 0;
  int      m_globalNoDomains = 1;
  MPI_Info m_mpiInfo         = MPI_INFO_NULL;
};

extern Information g_mpiInformation; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/// Return global domain id
inline auto globalDomainId() -> int { return g_mpiInformation.m_globalDomainId; }
/// Return global number of domains
inline auto globalNoDomains() -> int { return g_mpiInformation.m_globalNoDomains; }
/// Return global MPI information
inline auto globalMpiInfo() -> const MPI_Info& { return g_mpiInformation.m_mpiInfo; }
} // namespace MPI

#endif // GRIDGENERATOR_GLOBALMPI_H
