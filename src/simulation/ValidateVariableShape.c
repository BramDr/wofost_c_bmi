#include "simulation.h"
#include <netcdf.h>

void ValidateVariableShape(const int ncid, const int varid,
                           const size_t shape[NR_DOMAIN_DIMENSIONS]) {
  int status, dim_size, *var_dimid;
  size_t var_dimlen;

  if ((status = nc_inq_varndims(ncid, varid, &dim_size)) != NC_NOERR)
    ERR("Cannot get number of dimensions for variable: %s.",
        nc_strerror(status));

  if (dim_size < NR_DOMAIN_DIMENSIONS)
    ERR("Variable must have at least %d dimensions, but has %d.",
        NR_DOMAIN_DIMENSIONS, dim_size);

  var_dimid = malloc(dim_size * sizeof(int));
  if (var_dimid == NULL)
    ERR("Could not allocate memory for dimension IDs.");

  if ((status = nc_inq_vardimid(ncid, varid, var_dimid)) != NC_NOERR)
    ERR("Cannot get dimension IDs for variable: %s.", nc_strerror(status));

  for (int i = 0; i < NR_DOMAIN_DIMENSIONS; i++) {
    int inner_i = i + (dim_size - NR_DOMAIN_DIMENSIONS);
    if ((status = nc_inq_dimlen(ncid, var_dimid[inner_i], &var_dimlen)) !=
        NC_NOERR)
      ERR("Cannot get length of dimension %d for variable: %s.", inner_i,
          nc_strerror(status));

    if (var_dimlen != shape[i])
      ERR("Dimension %d of variable has length %zu, but expected %zu.", i,
          var_dimlen, shape[i]);
  }

  free(var_dimid);
}
