#include "simulation.h"
#include <math.h>
#include <netcdf.h>
#include <string.h>

static void InitializeDomainMeta(const NetCDFConfig *config, size_t *size,
                                 size_t (*shape)[NR_DOMAIN_DIMENSIONS],
                                 double **latitudes, double **longitudes,
                                 double *resolution) {
  DBG("InitializeDomainMeta");

  int status, ncid, lat_dimid, lon_dimid, lat_varid, lon_varid;

  if ((status = nc_open(config->file_path, NC_NOWRITE, &ncid)) != NC_NOERR)
    ERR("Cannot open domain file %s: %s.", config->file_path,
        nc_strerror(status));
  if ((status = nc_inq_dimid(ncid, config->latitude_name, &lat_dimid)) !=
      NC_NOERR)
    ERR("Cannot find latitude dimension %s: %s.", config->latitude_name,
        nc_strerror(status));
  if ((status = nc_inq_dimid(ncid, config->longitude_name, &lon_dimid)) !=
      NC_NOERR)
    ERR("Cannot find longitude dimension %s: %s.", config->longitude_name,
        nc_strerror(status));

  if ((status = nc_inq_dimlen(ncid, lat_dimid, &(*shape)[0])) != NC_NOERR)
    ERR("Cannot read latitude dimension length: %s.", nc_strerror(status));
  if ((status = nc_inq_dimlen(ncid, lon_dimid, &(*shape)[1])) != NC_NOERR)
    ERR("Cannot read longitude dimension length: %s.", nc_strerror(status));

  if ((*shape)[0] <= 1 || (*shape)[1] <= 1)
    ERR("Latitude and longitude dimensions must be greater than one.");
  /* Check if shape fits in size_t */
  if ((*shape)[0] > DOMAIN_DIMENSION_SIZE_MAX ||
      (*shape)[1] > DOMAIN_DIMENSION_SIZE_MAX)
    ERR("Latitude and longitude dimensions are too large.");

  *size = (*shape)[0] * (*shape)[1];
  *latitudes = malloc((*shape)[0] * sizeof(**latitudes));
  if (*latitudes == NULL)
    ERR("Could not allocate memory for Latitudes.");
  *longitudes = malloc((*shape)[1] * sizeof(**longitudes));
  if (*longitudes == NULL)
    ERR("Could not allocate memory for Longitudes.");

  if ((status = nc_inq_varid(ncid, config->latitude_name, &lat_varid)) !=
      NC_NOERR)
    ERR("Cannot find latitude variable %s: %s.", config->latitude_name,
        nc_strerror(status));
  if ((status = nc_inq_varid(ncid, config->longitude_name, &lon_varid)) !=
      NC_NOERR)
    ERR("Cannot find longitude variable %s: %s.", config->longitude_name,
        nc_strerror(status));
  if ((status = nc_get_var_double(ncid, lat_varid, *latitudes)) != NC_NOERR)
    ERR("Cannot read latitude variable values: %s.", nc_strerror(status));
  if ((status = nc_get_var_double(ncid, lon_varid, *longitudes)) != NC_NOERR)
    ERR("Cannot read longitude variable values: %s.", nc_strerror(status));

  double lat_resolution = fabs(
      ((*latitudes)[(*shape)[0] - 1] - (*latitudes)[0]) / ((*shape)[0] - 1));
  double lon_resolution = fabs(
      ((*longitudes)[(*shape)[1] - 1] - (*longitudes)[0]) / ((*shape)[1] - 1));
  if (fabs(lat_resolution - lon_resolution) > 1e-6)
    ERR("Latitude and longitude resolutions are not equal: %lf vs %lf.",
        lat_resolution, lon_resolution);
  *resolution = lat_resolution;

  if ((status = nc_close(ncid)) != NC_NOERR)
    ERR("Cannot close mask file %s: %s.", config->file_path,
        nc_strerror(status));
}

static void InitializeDomainData(const size_t size,
                                 const size_t shape[NR_DOMAIN_DIMENSIONS],
                                 const double latitudes[],
                                 const double longitudes[], DomUnit **grid) {
  DBG("InitializeDomainData");

  *grid = malloc(size * sizeof(**grid));
  if (*grid == NULL)
    ERR("Could not allocate memory for Grid.");

  for (size_t i = 0; i < size; i++) {
    Location *loc = &(*grid)[i].loc;
    Weather *met = &(*grid)[i].met;

    size_t lat_index = i / shape[1];
    size_t lon_index = i % shape[1];
    loc->Latitude = latitudes[lat_index];
    loc->Longitude = longitudes[lon_index];
    loc->AngstA = 0.4885 - 0.0052 * loc->Latitude;
    loc->AngstB = 0.1563 + 0.0074 * loc->Longitude;
    loc->Altitude = 100; // TODO: temporary needs to be fixed

    memset(met, 0, sizeof(*met));
  }
}

void InitializeDomainUnits(void) {
  DBG("InitializeDomainUnits");

  NetCDFConfig *template_config;

  template_config = &Configuration->weather_files[WEATHER_TMIN];
  InitializeDomainMeta(template_config, &DomainSize, &DomainShape, &Latitudes,
                       &Longitudes, &Resolution);
  InitializeDomainData(DomainSize, DomainShape, Latitudes, Longitudes,
                       &DomGrid);
}

void FinalizeDomainUnits(void) {
  DBG("FinalizeDomainUnits");

  free(Latitudes);
  free(Longitudes);
  free(DomGrid);
  Latitudes = NULL;
  Longitudes = NULL;
  DomGrid = NULL;
}
