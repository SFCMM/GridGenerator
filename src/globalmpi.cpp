#include "globalmpi.h"
#include <array>

MPI::Information MPI::g_mpiInformation; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)


/// \brief Print all information of the given MPI_Info object.
void printMpiInfo(MPI_Info& mpiInfo) {
  int nkeys = 0;

  MPI_Info_get_nkeys(mpiInfo, &nkeys);
  std::cerr << "MPI Info: nkeys = " << nkeys << std::endl;
  for(int i = 0; i < nkeys; i++) {
    std::array<GChar, MPI_MAX_INFO_KEY> key{};
    std::array<GChar, MPI_MAX_INFO_VAL> value{};
    int                                 valuelen = 0;
    int                                 flag     = 0;

    MPI_Info_get_nthkey(mpiInfo, i, &key[0]);
    MPI_Info_get_valuelen(mpiInfo, &key[0], &valuelen, &flag);
    MPI_Info_get(mpiInfo, &key[0], valuelen + 1, &value[0], &flag);
    std::cerr << "MPI Info: [" << i << "] key = " << key.data() << ", value = " << value.data() << std::endl;
  }
}