#include "simulation.h"
#include <assert.h>
#include <math.h>
#include <netcdf.h>
#include <string.h>

static void InitializeSimulationMeta(const NetCDFConfig *config,
                                     const size_t domain_size,
                                     const double west, const double east,
                                     const double south, const double north,
                                     const size_t shape[NR_DOMAIN_DIMENSIONS],
                                     size_t *active_size,
                                     size_t **active_index) {

  NetCDFMeta meta;
  InitializeNetCDFMeta(config->file_path, config->variable_name, &meta);
  DeriveNetCDFMetaSpace(west, east, south, north, shape, &meta);

  float *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR("Could not allocate memory for mask data.");

  ReadNetCDFMetaFloat(&meta, -1.0f, data, 0);
  FreeNetCDFMeta(&meta);

  *active_size = 0;
  for (size_t i = 0; i < domain_size; i++) {
    if (data[i] <= 0)
      continue;
    (*active_size)++;
  }

  *active_index = malloc(*active_size * sizeof(**active_index));
  if (*active_index == NULL)
    ERR("Could not allocate memory for active_index.");

  size_t idx = 0;
  for (size_t i = 0; i < domain_size; i++) {
    if (data[i] <= 0)
      continue;
    (*active_index)[idx++] = i;
  }

  free(data);
}

static void InitializeSimulationMetas(
    const Config *config, const size_t domain_size, const double west,
    const double east, const double south, const double north,
    const size_t shape[NR_DOMAIN_DIMENSIONS], const size_t crop_size,
    size_t **active_size, size_t ***active_index) {

  *active_size = malloc(crop_size * sizeof(**active_size));
  if (*active_size == NULL)
    ERR("Could not allocate memory for active_size.");
  *active_index = malloc(crop_size * sizeof(**active_index));
  if (*active_index == NULL)
    ERR("Could not allocate memory for active_index.");

  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];
    NetCDFConfig *netcdf_config = &crop_config->domain_files[DOMAIN_MASK];
    InitializeSimulationMeta(netcdf_config, domain_size, west, east, south,
                             north, shape, &(*active_size)[i],
                             &(*active_index)[i]);
  }
}

static void InitializeSimulationData(const Config *config,
                                     const size_t crop_size,
                                     const size_t *active_size,
                                     const size_t **active_index,
                                     DomUnit *domain_grid, SimUnit ***grid) {

  *grid = malloc(crop_size * sizeof(**grid));
  if (*grid == NULL)
    ERR("Could not allocate memory for Grid.");

  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];

    SimUnit template;
    memset(&template, 0, sizeof(template));
    template.start = crop_config->plant_date;
    template.emergence = crop_config->emergence;
    GetCropData(&template.crp, crop_config->crop_file);
    GetSiteData(&template.ste, crop_config->site_file);
    GetManagement(&template.mng, crop_config->management_file);
    GetSoilData(&template.soil, crop_config->soil_file);

    (*grid)[i] = malloc(active_size[i] * sizeof(SimUnit));
    if ((*grid)[i] == NULL)
      ERR("Could not allocate memory for Grid[%zu].", i);

    for (size_t j = 0; j < active_size[i]; j++) {
      SimUnit *unit = &(*grid)[i][j];
      CopySimUnit(&template, unit);

      size_t index = active_index[i][j];
      DomUnit *dom_unit = &domain_grid[index];
      unit->dom = dom_unit;
    }
  }
}

static void ReadSimulationPlantDateData(
    const NetCDFConfig *config, const size_t domain_size, double west,
    double east, double south, double north,
    const size_t shape[NR_DOMAIN_DIMENSIONS], const size_t active_size,
    const size_t *active_index, SimUnit *grid) {

  NetCDFMeta meta;
  InitializeNetCDFMeta(config->file_path, config->variable_name, &meta);
  DeriveNetCDFMetaSpace(west, east, south, north, shape, &meta);

  int *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR("Could not allocate memory for plant date data.");

  ReadNetCDFMetaInt(&meta, -1, data, 0);
  FreeNetCDFMeta(&meta);

  for (size_t i = 0; i < active_size; i++) {
    size_t index = active_index[i];
    int element = data[index];
    if (element == -1)
      ERR("Missing value for plant date variable %s at index %zu.",
          config->variable_name, index);
    if (element < 0)
      ERR("Negative value for plant date variable %s at index %zu.",
          config->variable_name, index);
    if (element > 365)
      ERR("Plant date variable %s at index %zu exceeds 365 days.",
          config->variable_name, index);
    grid[i].start = element;
  }

  free(data);
}

static void ReadSimulationTsum1Data(const NetCDFConfig *config,
                                    const size_t domain_size, double west,
                                    double east, double south, double north,
                                    const size_t shape[NR_DOMAIN_DIMENSIONS],
                                    const size_t active_size,
                                    const size_t *active_index, SimUnit *grid) {

  NetCDFMeta meta;
  InitializeNetCDFMeta(config->file_path, config->variable_name, &meta);
  DeriveNetCDFMetaSpace(west, east, south, north, shape, &meta);

  float *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR("Could not allocate memory for tsum1 data.");

  ReadNetCDFMetaFloat(&meta, -1.0f, data, 0);
  FreeNetCDFMeta(&meta);

  for (size_t i = 0; i < active_size; i++) {
    size_t index = active_index[i];
    float element = data[index];
    if (element == -1.0f)
      ERR("Missing value for tsum1 variable %s at index %zu.",
          config->variable_name, index);
    if (element < 0)
      ERR("Negative value for tsum1 variable %s at index %zu.",
          config->variable_name, index);
    grid[i].crp.prm.TempSum1 = element;
  }

  free(data);
}

static void ReadSimulationTsum2Data(const NetCDFConfig *config,
                                    const size_t domain_size, double west,
                                    double east, double south, double north,
                                    const size_t shape[NR_DOMAIN_DIMENSIONS],
                                    const size_t active_size,
                                    const size_t *active_index, SimUnit *grid) {

  NetCDFMeta meta;
  InitializeNetCDFMeta(config->file_path, config->variable_name, &meta);
  DeriveNetCDFMetaSpace(west, east, south, north, shape, &meta);

  float *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR("Could not allocate memory for tsum2 data.");

  ReadNetCDFMetaFloat(&meta, -1.0f, data, 0);
  FreeNetCDFMeta(&meta);

  for (size_t i = 0; i < active_size; i++) {
    size_t index = active_index[i];
    float element = data[index];
    if (element == -1.0f)
      ERR("Missing value for tsum2 variable %s at index %zu.",
          config->variable_name, index);
    if (element < 0)
      ERR("Negative value for tsum2 variable %s at index %zu.",
          config->variable_name, index);
    grid[i].crp.prm.TempSum2 = element;
  }

  free(data);
}

static void
ReadSimulationSpatialData(const Config *config, const size_t domain_size,
                          double west, double east, double south, double north,
                          const size_t shape[NR_DOMAIN_DIMENSIONS],
                          const size_t crop_size, const size_t *active_size,
                          const size_t **active_index, SimUnit **grid) {

  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];

    NetCDFConfig *netcdf_config;

    netcdf_config = &crop_config->domain_files[DOMAIN_PLANT_DATE];
    ReadSimulationPlantDateData(netcdf_config, domain_size, west, east, south,
                                north, shape, active_size[i], active_index[i],
                                grid[i]);

    netcdf_config = &crop_config->domain_files[DOMAIN_TSUM1];
    if (strlen(netcdf_config->file_path) > 0) {
      ReadSimulationTsum1Data(netcdf_config, domain_size, west, east, south,
                              north, shape, active_size[i], active_index[i],
                              grid[i]);
    }

    netcdf_config = &crop_config->domain_files[DOMAIN_TSUM2];
    if (strlen(netcdf_config->file_path) > 0) {
      ReadSimulationTsum2Data(netcdf_config, domain_size, west, east, south,
                              north, shape, active_size[i], active_index[i],
                              grid[i]);
    }
  }
}

void InitializeSimulationUnits(void) {
  DBG("InitializeSimulationUnits");

  CropSize = Configuration->CropSize;

  CropNames = malloc(CropSize * sizeof(*CropNames));
  if (CropNames == NULL)
    ERR("Could not allocate memory for crop_names.");
  for (size_t i = 0; i < CropSize; i++) {
    CropConfig *crop_config = &Configuration->crop_configurations[i];
    CropNames[i] = strdup(crop_config->crop_name);
    if (CropNames[i] == NULL)
      ERR("Could not allocate memory for crop_names[%zu].", i);
  }

  InitializeSimulationMetas(Configuration, DomainSize, West, East, South, North,
                            DomainShape, CropSize, &ActiveSize, &ActiveIndex);

  InitializeSimulationData(Configuration, CropSize, ActiveSize,
                           (const size_t **)ActiveIndex, DomGrid, &SimGrid);

  ReadSimulationSpatialData(Configuration, DomainSize, West, East, South, North,
                            DomainShape, CropSize, ActiveSize,
                            (const size_t **)ActiveIndex, SimGrid);
}

void FinalizeSimulationUnits(void) {
  DBG("FinalizeSimulationUnits");

  for (size_t i = 0; i < CropSize; i++) {
    for (size_t j = 0; j < ActiveSize[i]; j++) {
      SUnit = &SimGrid[i][j];
      Clean();
    }
    free(ActiveIndex[i]);
    free(SimGrid[i]);
    free(CropNames[i]);
    ActiveIndex[i] = NULL;
    SimGrid[i] = NULL;
    CropNames[i] = NULL;
  }
  free(ActiveIndex);
  free(SimGrid);
  free(ActiveSize);
  free(CropNames);
  ActiveIndex = NULL;
  SimGrid = NULL;
  ActiveSize = NULL;
  CropNames = NULL;
}
