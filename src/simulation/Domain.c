#include "simulation.h"
#include <math.h>
#include <netcdf.h>
#include <string.h>

static void InitializeDomainMeta(const NetCDFConfig *config, size_t *size,
                                 size_t (*shape)[NR_DOMAIN_DIMENSIONS],
                                 double **latitudes, double **longitudes,
                                 double *resolution, double *west, double *east,
                                 double *north, double *south) {
  DBG("InitializeDomainMeta");
  NetCDFMeta meta;
  InitializeNetCDFMeta(config->file_path, config->variable_name, &meta);

  *size = meta.lat_len * meta.lon_len;
  (*shape)[0] = meta.lat_len;
  (*shape)[1] = meta.lon_len;

  *latitudes = malloc(meta.lat_len * sizeof(**latitudes));
  if (*latitudes == NULL)
    ERR("Could not allocate memory for latitudes.");
  *longitudes = malloc(meta.lon_len * sizeof(**longitudes));
  if (*longitudes == NULL)
    ERR("Could not allocate memory for longitudes.");

  memcpy(*latitudes, meta.lat, meta.lat_len * sizeof(**latitudes));
  memcpy(*longitudes, meta.lon, meta.lon_len * sizeof(**longitudes));

  FreeNetCDFMeta(&meta);

  double lat_resolution = fabs(
      ((*latitudes)[(*shape)[0] - 1] - (*latitudes)[0]) / ((*shape)[0] - 1));
  double lon_resolution = fabs(
      ((*longitudes)[(*shape)[1] - 1] - (*longitudes)[0]) / ((*shape)[1] - 1));
  if (fabs(lat_resolution - lon_resolution) > 1e-6)
    ERR("Latitude and longitude resolutions are not equal: %lf vs %lf.",
        lat_resolution, lon_resolution);
  *resolution = lat_resolution;

  *west = fmin((*longitudes)[0], (*longitudes)[(*shape)[1] - 1]);
  *east = fmax((*longitudes)[0], (*longitudes)[(*shape)[1] - 1]);
  *north = fmax((*latitudes)[0], (*latitudes)[(*shape)[0] - 1]);
  *south = fmin((*latitudes)[0], (*latitudes)[(*shape)[0] - 1]);
  *west = (*west) - 0.5 * (*resolution);
  *east = (*east) + 0.5 * (*resolution);
  *north = (*north) + 0.5 * (*resolution);
  *south = (*south) - 0.5 * (*resolution);
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

  InitializeDomainMeta(&Configuration->area_file, &DomainSize, &DomainShape,
                       &Latitudes, &Longitudes, &Resolution, &West, &East,
                       &North, &South);
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
