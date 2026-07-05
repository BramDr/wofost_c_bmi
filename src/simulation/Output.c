#include "output.h"
#include "simulation.h"
#include <netcdf.h>

static void InitializeOutputData(const size_t domain_size,
                                 const size_t crop_size,
                                 NetCDFMeta **metas[OUTPUT_NTYPES],
                                 float **float_data, int **int_data) {
  *float_data = malloc(domain_size * sizeof(**float_data));
  if (*float_data == NULL)
    ERR(printf("Could not allocate memory for output float data."));

  *int_data = malloc(domain_size * sizeof(**int_data));
  if (*int_data == NULL)
    ERR(printf("Could not allocate memory for output int data."));

  *metas = malloc(crop_size * sizeof(**metas));
  if (*metas == NULL)
    ERR(printf("Could not allocate memory for output metadata."));
}

static void DefineVariableAttributes(const int ncid, const int varid,
                                     const char *units,
                                     const char *standard_name,
                                     const char *long_name) {
  int status;

  if ((status = nc_put_att_text(ncid, varid, "units", strlen(units), units)) !=
      NC_NOERR)
    ERR(printf("Cannot define attribute 'units' for variable in file: %s.",
               nc_strerror(status)));
  if ((status = nc_put_att_text(ncid, varid, "standard_name",
                                strlen(standard_name), standard_name)) !=
      NC_NOERR)
    ERR(printf(
        "Cannot define attribute 'standard_name' for variable in file: %s.",
        nc_strerror(status)));
  if ((status = nc_put_att_text(ncid, varid, "long_name", strlen(long_name),
                                long_name)) != NC_NOERR)
    ERR(printf("Cannot define attribute 'long_name' for variable in file: %s.",
               nc_strerror(status)));
}

static void StartOutput(const Config *configuration, const size_t domain_size,
                        const size_t domain_shape[NR_DOMAIN_DIMENSIONS],
                        const double latitudes[], const double longitudes[],
                        const size_t crop_size,
                        NetCDFMeta *metas[OUTPUT_NTYPES]) {
  int status;

  for (size_t i = 0; i < crop_size; i++) {
    CropConfig *crop_config = &configuration->crop_configurations[i];
    char *crop_name = crop_config->crop_name;

    for (size_t j = 0; j < OUTPUT_NTYPES; j++) {
      NetCDFMeta *meta = &metas[i][j];

      OutputVar *output_var = &OUTPUT_VARS[j];
      char *var_name = output_var->name;

      // Construct the output file path as
      // [output_path]/[crop_name]_[var_name].nc
      char output_file[MAX_STRING] = {0};
      snprintf(output_file, MAX_STRING, "%s/%s_%s.nc",
               configuration->output_path, crop_name, var_name);

      if ((status = nc_create(output_file, NC_NETCDF4, &meta->ncid)) !=
          NC_NOERR)
        ERR(printf("Cannot create output file %s: %s.", output_file,
                   nc_strerror(status)));

      if ((status = nc_def_dim(meta->ncid, TIME_VAR.name, NC_UNLIMITED,
                               &meta->time_dimid)) != NC_NOERR)
        ERR(printf("Cannot define time dimension in file %s: %s.", output_file,
                   nc_strerror(status)));
      if ((status = nc_def_dim(meta->ncid, LAT_VAR.name, domain_shape[0],
                               &meta->lat_dimid)) != NC_NOERR)
        ERR(printf("Cannot define latitude dimension in file %s: %s.",
                   output_file, nc_strerror(status)));
      if ((status = nc_def_dim(meta->ncid, LON_VAR.name, domain_shape[1],
                               &meta->lon_dimid)) != NC_NOERR)
        ERR(printf("Cannot define longitude dimension in file %s: %s.",
                   output_file, nc_strerror(status)));

      /* Time variable */
      if ((status = nc_def_var(meta->ncid, TIME_VAR.name, TIME_VAR.type, 1,
                               &meta->time_dimid, &meta->time_varid)) !=
          NC_NOERR)
        ERR(printf("Cannot define variable %s in file %s: %s.", TIME_VAR.name,
                   output_file, nc_strerror(status)));
      DefineVariableAttributes(meta->ncid, meta->time_varid, TIME_VAR.units,
                               TIME_VAR.standard_name, TIME_VAR.long_name);
      if ((status = nc_put_att_text(meta->ncid, meta->time_varid, "calendar",
                                    strlen("standard"), "standard")) !=
          NC_NOERR)
        ERR(printf("Cannot define attribute 'calendar' for variable %s in file "
                   "%s: %s.",
                   TIME_VAR.name, output_file, nc_strerror(status)));
      if ((status = nc_put_att_text(meta->ncid, meta->time_varid, "axis",
                                    strlen("T"), "T")) != NC_NOERR)
        ERR(printf(
            "Cannot define attribute 'axis' for variable %s in file %s: %s.",
            TIME_VAR.name, output_file, nc_strerror(status)));

      /* Latitude variable */
      if ((status = nc_def_var(meta->ncid, LAT_VAR.name, LAT_VAR.type, 1,
                               &meta->lat_dimid, &meta->lat_varid)) != NC_NOERR)
        ERR(printf("Cannot define variable %s in file %s: %s.", LAT_VAR.name,
                   output_file, nc_strerror(status)));
      DefineVariableAttributes(meta->ncid, meta->lat_varid, LAT_VAR.units,
                               LAT_VAR.standard_name, LAT_VAR.long_name);
      if ((status = nc_put_att_text(meta->ncid, meta->lat_varid, "axis",
                                    strlen("Y"), "Y")) != NC_NOERR)
        ERR(printf(
            "Cannot define attribute 'axis' for variable %s in file %s: %s.",
            LAT_VAR.name, output_file, nc_strerror(status)));

      /* Longitude variable */
      if ((status = nc_def_var(meta->ncid, LON_VAR.name, LON_VAR.type, 1,
                               &meta->lon_dimid, &meta->lon_varid)) != NC_NOERR)
        ERR(printf("Cannot define variable %s in file %s: %s.", LON_VAR.name,
                   output_file, nc_strerror(status)));
      DefineVariableAttributes(meta->ncid, meta->lon_varid, LON_VAR.units,
                               LON_VAR.standard_name, LON_VAR.long_name);
      if ((status = nc_put_att_text(meta->ncid, meta->lon_varid, "axis",
                                    strlen("X"), "X")) != NC_NOERR)
        ERR(printf(
            "Cannot define attribute 'axis' for variable %s in file %s: %s.",
            LON_VAR.name, output_file, nc_strerror(status)));

      /* Output variable */
      int dimids[NR_DOMAIN_DIMENSIONS + 1] = {meta->time_dimid, meta->lat_dimid,
                                              meta->lon_dimid};
      if ((status = nc_def_var(meta->ncid, output_var->name, output_var->type,
                               NR_DOMAIN_DIMENSIONS + 1, dimids,
                               &meta->varid)) != NC_NOERR)
        ERR(printf("Cannot define variable %s in file %s: %s.",
                   output_var->name, output_file, nc_strerror(status)));
      DefineVariableAttributes(meta->ncid, meta->varid, output_var->units,
                               output_var->standard_name,
                               output_var->long_name);

      if (output_var->type == NC_FLOAT) {
        float fill_value = NC_FILL_FLOAT;
        if ((status = nc_def_var_fill(meta->ncid, meta->varid, NC_FILL,
                                      &fill_value)) != NC_NOERR)
          ERR(printf("Cannot define fill value for variable %s in file %s: %s.",
                     output_var->name, output_file, nc_strerror(status)));
      } else if (output_var->type == NC_INT) {
        int fill_value = NC_FILL_INT;
        if ((status = nc_def_var_fill(meta->ncid, meta->varid, NC_FILL,
                                      &fill_value)) != NC_NOERR)
          ERR(printf("Cannot define fill value for variable %s in file %s: %s.",
                     output_var->name, output_file, nc_strerror(status)));
      } else {
        ERR(printf("Unsupported variable type %d for variable %s in file %s.",
                   output_var->type, output_var->name, output_file));
      }

      if ((status = nc_enddef(meta->ncid)) != NC_NOERR)
        ERR(printf("Cannot end definition mode for file %s: %s.", output_file,
                   nc_strerror(status)));

      if ((status = nc_put_var_double(meta->ncid, meta->lat_varid,
                                      latitudes)) != NC_NOERR)
        ERR(printf("Cannot write data for variable %s in file %s: %s.",
                   LAT_VAR.name, output_file, nc_strerror(status)));
      if ((status = nc_put_var_double(meta->ncid, meta->lon_varid,
                                      longitudes)) != NC_NOERR)
        ERR(printf("Cannot write data for variable %s in file %s: %s.",
                   LON_VAR.name, output_file, nc_strerror(status)));
    }
  }
}

static void WriteOutputFloatData(const OutputVar *var, const float data[],
                                 NetCDFMeta *meta) {
  int status;
  size_t start[NR_DOMAIN_DIMENSIONS + 1] = {meta->time_len, 0, 0};
  size_t count[NR_DOMAIN_DIMENSIONS + 1] = {1, meta->lat_len, meta->lon_len};

  if ((status = nc_put_vara_float(meta->ncid, meta->varid, start, count,
                                  data)) != NC_NOERR)
    ERR(printf("Cannot write data for variable %s in file %d: %s.", var->name,
               meta->ncid, nc_strerror(status)));

  int time = CurrentTime / TIME_STEP; // Convert seconds to days
  size_t time_start[1] = {meta->time_len};
  size_t time_count[1] = {1};
  if ((status = nc_put_vara_int(meta->ncid, meta->time_varid, time_start,
                                time_count, &time)) != NC_NOERR)
    ERR(printf("Cannot write data for variable %s in file %d: %s.",
               TIME_VAR.name, meta->ncid, nc_strerror(status)));

  meta->time_len++;
}

static void WriteOutputIntData(const OutputVar *var, const int data[],
                               NetCDFMeta *meta) {
  int status;
  size_t start[NR_DOMAIN_DIMENSIONS + 1] = {meta->time_len, 0, 0};
  size_t count[NR_DOMAIN_DIMENSIONS + 1] = {1, meta->lat_len, meta->lon_len};

  if ((status = nc_put_vara_int(meta->ncid, meta->varid, start, count, data)) !=
      NC_NOERR)
    ERR(printf("Cannot write data for variable %s in file %d: %s.", var->name,
               meta->ncid, nc_strerror(status)));

  int time = CurrentTime / TIME_STEP; // Convert seconds to days
  size_t time_start[1] = {meta->time_len};
  size_t time_count[1] = {1};
  if ((status = nc_put_vara_int(meta->ncid, meta->time_varid, time_start,
                                time_count, &time)) != NC_NOERR)
    ERR(printf("Cannot write data for variable %s in file %d: %s.",
               TIME_VAR.name, meta->ncid, nc_strerror(status)));

  meta->time_len++;
}

static void WriteOutputData(const size_t domain_size, const size_t crop_size,
                            const size_t active_size[],
                            const size_t *active_index[],
                            NetCDFMeta *metas[OUTPUT_NTYPES],
                            float float_data[], int int_data[],
                            SimUnit *grid[]) {

  for (size_t i = 0; i < crop_size; i++) {

    for (size_t j = 0; j < OUTPUT_NTYPES; j++) {
      NetCDFMeta *meta = &metas[i][j];
      const OutputVar *output_var = &OUTPUT_VARS[j];

      if (output_var->type == NC_FLOAT) {
        for (size_t k = 0; k < domain_size; k++) {
          float_data[k] = NC_FILL_FLOAT;
        }
      } else if (output_var->type == NC_INT) {
        for (size_t k = 0; k < domain_size; k++) {
          int_data[k] = NC_FILL_INT;
        }
      } else {
        ERR(printf("Unsupported variable type %d for variable %s.",
                   output_var->type, output_var->name));
      }

      for (size_t k = 0; k < active_size[i]; k++) {
        size_t index = active_index[i][k];
        SimUnit *unit = &grid[i][k];

        switch (j) {
        case OUTPUT_GROWTH_DAY:
          int_data[index] = unit->crp.GrowthDay;
          break;
        case OUTPUT_DEVELOPMENT:
          float_data[index] = unit->crp.st.Development;
          break;
        case OUTPUT_ROOT_BIOMASS:
          float_data[index] = unit->crp.st.roots;
          break;
        case OUTPUT_LEAVES_BIOMASS:
          float_data[index] = unit->crp.st.leaves;
          break;
        case OUTPUT_STEMS_BIOMASS:
          float_data[index] = unit->crp.st.stems;
          break;
        case OUTPUT_STORAGE_BIOMASS:
          float_data[index] = unit->crp.st.storage;
          break;
        case OUTPUT_ROOT_DEPTH:
          float_data[index] = unit->crp.st.RootDepth;
          break;
        case OUTPUT_LEAF_AREA_INDEX:
          float_data[index] = unit->crp.st.LAI;
          break;
        case OUTPUT_STRESS:
          float_data[index] =
              min(unit->crp.NutrientStress, unit->soil.WaterStress);
          break;
        case OUTPUT_WATER_STRESS:
          float_data[index] = unit->soil.WaterStress;
          break;
        case OUTPUT_HEAT_STRESS:
          /* HeatStress() is not yet implemented; leave as fill value. */
          break;
        case OUTPUT_NUTRIENT_STRESS:
          float_data[index] = unit->crp.NutrientStress;
          break;
        case OUTPUT_ROOT_DEAD:
          float_data[index] = unit->crp.dst.roots;
          break;
        case OUTPUT_LEAVES_DEAD:
          float_data[index] = unit->crp.dst.leaves;
          break;
        case OUTPUT_STEMS_DEAD:
          float_data[index] = unit->crp.dst.stems;
          break;
        case OUTPUT_ROOT_N_CONTENT:
          float_data[index] = unit->crp.N_st.roots;
          break;
        case OUTPUT_LEAVES_N_CONTENT:
          float_data[index] = unit->crp.N_st.leaves;
          break;
        case OUTPUT_STEMS_N_CONTENT:
          float_data[index] = unit->crp.N_st.stems;
          break;
        case OUTPUT_STORAGE_N_CONTENT:
          float_data[index] = unit->crp.N_st.storage;
          break;
        case OUTPUT_ROOT_P_CONTENT:
          float_data[index] = unit->crp.P_st.roots;
          break;
        case OUTPUT_LEAVES_P_CONTENT:
          float_data[index] = unit->crp.P_st.leaves;
          break;
        case OUTPUT_STEMS_P_CONTENT:
          float_data[index] = unit->crp.P_st.stems;
          break;
        case OUTPUT_STORAGE_P_CONTENT:
          float_data[index] = unit->crp.P_st.storage;
          break;
        case OUTPUT_ROOT_K_CONTENT:
          float_data[index] = unit->crp.K_st.roots;
          break;
        case OUTPUT_LEAVES_K_CONTENT:
          float_data[index] = unit->crp.K_st.leaves;
          break;
        case OUTPUT_STEMS_K_CONTENT:
          float_data[index] = unit->crp.K_st.stems;
          break;
        case OUTPUT_STORAGE_K_CONTENT:
          float_data[index] = unit->crp.K_st.storage;
          break;
        default:
          ERR(printf("Unknown output variable index %d.", j));
        }
      }

      if (output_var->type == NC_FLOAT) {
        WriteOutputFloatData(output_var, float_data, meta);
      } else if (output_var->type == NC_INT) {
        WriteOutputIntData(output_var, int_data, meta);
      }
    }
  }
}

static void StopOutput(const size_t crop_size,
                       NetCDFMeta *metas[OUTPUT_NTYPES]) {
  int status;
  for (size_t i = 0; i < crop_size; i++) {
    for (size_t j = 0; j < OUTPUT_NTYPES; j++) {
      NetCDFMeta *meta = &metas[i][j];
      if ((status = nc_close(meta->ncid)) != NC_NOERR)
        ERR(printf("Cannot close output file %d: %s.", meta->ncid,
                   nc_strerror(status)));
    }
  }
}

static void FinalizeOutputData(float *float_data[], int *int_data[],
                               NetCDFMeta **metas[OUTPUT_NTYPES]) {
  free(*float_data);
  free(*int_data);
  free(*metas);
  *float_data = NULL;
  *int_data = NULL;
  *metas = NULL;
}

void InitializeOutput() {
  InitializeOutputData(DomainSize, CropSize, &OutputMetas, &OutputFloatData,
                       &OutputIntData);
  StartOutput(Configuration, DomainSize, DomainShape, Latitudes, Longitudes,
              CropSize, OutputMetas);
}

void UpdateOutput() {
  WriteOutputData(DomainSize, CropSize, ActiveSize, ActiveIndex, OutputMetas,
                  OutputFloatData, OutputIntData, SimGrid);
}

void FinalizeOutput() {
  StopOutput(CropSize, OutputMetas);
  FinalizeOutputData(&OutputFloatData, &OutputIntData, &OutputMetas);
}