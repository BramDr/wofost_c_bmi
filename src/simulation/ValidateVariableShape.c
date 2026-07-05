#include "simulation.h"
#include <netcdf.h>

void ValidateVariableShape(const int ncid, const int varid,
                           const size_t shape[NR_DOMAIN_DIMENSIONS]) {
  int status, dim_size, var_dimid[NR_DOMAIN_DIMENSIONS];
  size_t var_dimlen;

  if ((status = nc_inq_varndims(ncid, varid, &dim_size)) != NC_NOERR)
    ERR(printf("Cannot get number of dimensions for variable: %s.",
               nc_strerror(status)));

  if (dim_size != NR_DOMAIN_DIMENSIONS)
    ERR(printf("Variable must have %d dimensions, but has %d.",
               NR_DOMAIN_DIMENSIONS, dim_size));

  if ((status = nc_inq_vardimid(ncid, varid, var_dimid)) != NC_NOERR)
    ERR(printf("Cannot get dimension IDs for variable: %s.",
               nc_strerror(status)));

  for (int i = 0; i < dim_size; i++) {
    if ((status = nc_inq_dimlen(ncid, var_dimid[i], &var_dimlen)) != NC_NOERR)
      ERR(printf("Cannot get length of dimension %d for variable: %s.", i,
                 nc_strerror(status)));

    if (var_dimlen != shape[i])
      ERR(printf("Dimension %d of variable has length %zu, but expected %zu.",
                 i, var_dimlen, shape[i]));
  }
}
