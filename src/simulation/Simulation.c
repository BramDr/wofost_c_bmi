#include "simulation.h"
#include <assert.h>
#include <netcdf.h>
#include <string.h>

static void
InitializeSimulationMeta(const NetCDFConfig *config, const size_t domain_size,
                         const size_t domain_shape[NR_DOMAIN_DIMENSIONS],
                         size_t *active_size, size_t **active_index) {
  int status, ncid, varid;
  float *data;

  if ((status = nc_open(config->file_path, NC_NOWRITE, &ncid)) != NC_NOERR)
    ERR(printf("Cannot open mask file %s: %s.", config->file_path,
               nc_strerror(status)));
  if ((status = nc_inq_varid(ncid, config->variable_name, &varid)) != NC_NOERR)
    ERR(printf("Cannot find mask variable %s: %s.", config->variable_name,
               nc_strerror(status)));

  ValidateVariableShape(ncid, varid, domain_shape);

  data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR(printf("Could not allocate memory for mask data."));

  if ((status = nc_get_var_float(ncid, varid, &data[0])) != NC_NOERR)
    ERR(printf("Cannot read mask variable values: %s.", nc_strerror(status)));

  *active_size = 0;
  for (size_t i = 0; i < domain_size; i++) {
    if (data[i] <= 0)
      continue;
    (*active_size)++;
  }

  *active_index = malloc(*active_size * sizeof(**active_index));
  if (*active_index == NULL)
    ERR(printf("Could not allocate memory for active_index."));

  size_t idx = 0;
  for (size_t i = 0; i < domain_size; i++) {
    if (data[i] <= 0)
      continue;
    (*active_index)[idx++] = i;
  }

  if ((status = nc_close(ncid)) != NC_NOERR)
    ERR(printf("Cannot close mask file %s: %s.", config->file_path,
               nc_strerror(status)));

  free(data);
}

static void
InitializeSimulationMetas(const Config *config, const size_t domain_size,
                          const size_t domain_shape[NR_DOMAIN_DIMENSIONS],
                          const size_t crop_size, size_t **active_size,
                          size_t ***active_index) {
  *active_size = malloc(crop_size * sizeof(**active_size));
  if (*active_size == NULL)
    ERR(printf("Could not allocate memory for active_size."));
  *active_index = malloc(crop_size * sizeof(**active_index));
  if (*active_index == NULL)
    ERR(printf("Could not allocate memory for active_index."));

  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];
    NetCDFConfig *netcdf_config = &crop_config->domain_files[DOMAIN_MASK];
    InitializeSimulationMeta(netcdf_config, domain_size, domain_shape,
                             &(*active_size)[i], &(*active_index)[i]);
  }
}

static void InitializeSimulationData(const Config *config,
                                     const size_t crop_size,
                                     const size_t *active_size,
                                     const size_t **active_index,
                                     DomUnit *domain_grid, SimUnit ***grid) {
  *grid = malloc(crop_size * sizeof(**grid));
  if (*grid == NULL)
    ERR(printf("Could not allocate memory for Grid."));

  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];

    SimUnit template;
    memset(&template, 0, sizeof(template));
    template.emergence = crop_config->emergence;
    GetCropData(&template.crp, crop_config->crop_file);
    GetSiteData(&template.ste, crop_config->site_file);
    GetManagement(&template.mng, crop_config->management_file);
    GetSoilData(&template.soil, crop_config->soil_file);

    (*grid)[i] = malloc(active_size[i] * sizeof(SimUnit));
    if ((*grid)[i] == NULL)
      ERR(printf("Could not allocate memory for Grid[%zu].", i));

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
    const NetCDFConfig *config, const size_t domain_size,
    const size_t domain_shape[NR_DOMAIN_DIMENSIONS], const size_t active_size,
    const size_t *active_index, SimUnit *grid) {
  int status, ncid, varid;

  if ((status = nc_open(config->file_path, NC_NOWRITE, &ncid)) != NC_NOERR)
    ERR(printf("Cannot open plant date file %s: %s.", config->file_path,
               nc_strerror(status)));
  if ((status = nc_inq_varid(ncid, config->variable_name, &varid)) != NC_NOERR)
    ERR(printf("Cannot find plant date variable %s: %s.", config->variable_name,
               nc_strerror(status)));

  ValidateVariableShape(ncid, varid, domain_shape);

  int *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR(printf("Could not allocate memory for plant date data."));

  if ((status = nc_get_var_int(ncid, varid, &data[0])) != NC_NOERR)
    ERR(printf("Cannot read plant date variable values: %s.",
               nc_strerror(status)));

  for (size_t i = 0; i < active_size; i++) {
    size_t index = active_index[i];
    int element = data[index];
    assert(element >= 0 && element <= 365);
    grid[i].start = element;
  }

  if ((status = nc_close(ncid)) != NC_NOERR)
    ERR(printf("Cannot close plant date file %s: %s.", config->file_path,
               nc_strerror(status)));

  free(data);
}

static void
ReadSimulationTsum1Data(const NetCDFConfig *config, const size_t domain_size,
                        const size_t domain_shape[NR_DOMAIN_DIMENSIONS],
                        const size_t active_size, const size_t *active_index,
                        SimUnit *grid) {
  int status, ncid, varid;

  if ((status = nc_open(config->file_path, NC_NOWRITE, &ncid)) != NC_NOERR)
    ERR(printf("Cannot open tsum1 file %s: %s.", config->file_path,
               nc_strerror(status)));
  if ((status = nc_inq_varid(ncid, config->variable_name, &varid)) != NC_NOERR)
    ERR(printf("Cannot find tsum1 variable %s: %s.", config->variable_name,
               nc_strerror(status)));

  ValidateVariableShape(ncid, varid, domain_shape);

  float *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR(printf("Could not allocate memory for tsum1 data."));

  if ((status = nc_get_var_float(ncid, varid, &data[0])) != NC_NOERR)
    ERR(printf("Cannot read tsum1 variable values: %s.", nc_strerror(status)));

  for (size_t i = 0; i < active_size; i++) {
    size_t index = active_index[i];
    float element = data[index];
    assert(element >= 0);
    grid[i].crp.prm.TempSum1 = element;
  }

  if ((status = nc_close(ncid)) != NC_NOERR)
    ERR(printf("Cannot close tsum1 file %s: %s.", config->file_path,
               nc_strerror(status)));

  free(data);
}

static void
ReadSimulationTsum2Data(const NetCDFConfig *config, const size_t domain_size,
                        const size_t domain_shape[NR_DOMAIN_DIMENSIONS],
                        const size_t active_size, const size_t *active_index,
                        SimUnit *grid) {
  int status, ncid, varid;

  if ((status = nc_open(config->file_path, NC_NOWRITE, &ncid)) != NC_NOERR)
    ERR(printf("Cannot open tsum2 file %s: %s.", config->file_path,
               nc_strerror(status)));
  if ((status = nc_inq_varid(ncid, config->variable_name, &varid)) != NC_NOERR)
    ERR(printf("Cannot find tsum2 variable %s: %s.", config->variable_name,
               nc_strerror(status)));

  ValidateVariableShape(ncid, varid, domain_shape);

  float *data = malloc(domain_size * sizeof(*data));
  if (data == NULL)
    ERR(printf("Could not allocate memory for tsum2 data."));

  if ((status = nc_get_var_float(ncid, varid, &data[0])) != NC_NOERR)
    ERR(printf("Cannot read tsum2 variable values: %s.", nc_strerror(status)));

  for (size_t i = 0; i < active_size; i++) {
    size_t index = active_index[i];
    float element = data[index];
    assert(element >= 0);
    grid[i].crp.prm.TempSum2 = element;
  }

  if ((status = nc_close(ncid)) != NC_NOERR)
    ERR(printf("Cannot close tsum2 file %s: %s.", config->file_path,
               nc_strerror(status)));

  free(data);
}

static void
ReadSimulationSpatialData(const Config *config, const size_t domain_size,
                          const size_t domain_shape[NR_DOMAIN_DIMENSIONS],
                          const size_t crop_size, const size_t *active_size,
                          const size_t **active_index, SimUnit **grid) {
  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];

    NetCDFConfig *netcdf_config;

    netcdf_config = &crop_config->domain_files[DOMAIN_PLANT_DATE];
    ReadSimulationPlantDateData(netcdf_config, domain_size, domain_shape,
                                active_size[i], active_index[i], grid[i]);

    netcdf_config = &crop_config->domain_files[DOMAIN_TSUM1];
    ReadSimulationTsum1Data(netcdf_config, domain_size, domain_shape,
                            active_size[i], active_index[i], grid[i]);

    netcdf_config = &crop_config->domain_files[DOMAIN_TSUM2];
    ReadSimulationTsum2Data(netcdf_config, domain_size, domain_shape,
                            active_size[i], active_index[i], grid[i]);
  }
}

void InitializeSimulationUnits(void) {
  CropSize = Configuration->CropSize;

  InitializeSimulationMetas(Configuration, DomainSize, DomainShape, CropSize,
                            &ActiveSize, &ActiveIndex);

  InitializeSimulationData(Configuration, CropSize, ActiveSize,
                           (const size_t **)ActiveIndex, DomGrid, &SimGrid);

  ReadSimulationSpatialData(Configuration, DomainSize, DomainShape, CropSize,
                            ActiveSize, (const size_t **)ActiveIndex, SimGrid);
}

void FinalizeSimulationUnits(void) {
  for (size_t i = 0; i < CropSize; i++) {
    for (size_t j = 0; j < ActiveSize[i]; j++) {
      SUnit = &SimGrid[i][j];
      Clean();
    }
    free(ActiveIndex[i]);
    free(SimGrid[i]);
    ActiveIndex[i] = NULL;
    SimGrid[i] = NULL;
  }
  free(ActiveIndex);
  free(SimGrid);
  free(ActiveSize);
  ActiveIndex = NULL;
  SimGrid = NULL;
  ActiveSize = NULL;
}
