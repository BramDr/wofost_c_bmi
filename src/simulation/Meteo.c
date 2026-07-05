#include "simulation.h"
#include "time_utils.h"
#include <netcdf.h>

static void InitializeMeteoData(const Config *config, const size_t size,
                                const size_t shape[NR_DOMAIN_DIMENSIONS],
                                float **data) {
  int status, ncid, varid;

  for (int i = 0; i < WEATHER_NTYPES; i++) {
    NetCDFConfig *netcdf_config = &config->weather_files[i];

    if ((status = nc_open(netcdf_config->file_path, NC_NOWRITE, &ncid)) !=
        NC_NOERR)
      ERR(printf("Cannot open weather file %s: %s.", netcdf_config->file_path,
                 nc_strerror(status)));

    if ((status = nc_inq_varid(ncid, netcdf_config->variable_name, &varid)) !=
        NC_NOERR)
      ERR(printf("Cannot find weather variable %s in file %s: %s.",
                 netcdf_config->variable_name, netcdf_config->file_path,
                 nc_strerror(status)));

    ValidateVariableShape(ncid, varid, shape);

    if ((status = nc_close(ncid)) != NC_NOERR)
      ERR(printf("Cannot close weather file %s: %s.", netcdf_config->file_path,
                 nc_strerror(status)));
  }

  *data = malloc(size * sizeof(**data));
  if (*data == NULL)
    ERR(printf("Could not allocate memory for weather data."));
}

static void StartMeteo(const Config *config, const size_t size,
                       const size_t shape[NR_DOMAIN_DIMENSIONS],
                       NetCDFMeta metas[WEATHER_NTYPES]) {
  int status;

  for (int i = 0; i < WEATHER_NTYPES; i++) {
    NetCDFConfig *netcdf_config = &config->weather_files[i];
    NetCDFMeta *meta = &metas[i];

    if ((status = nc_open(netcdf_config->file_path, NC_NOWRITE, &meta->ncid)) !=
        NC_NOERR)
      ERR(printf("Cannot open weather file %s: %s.", netcdf_config->file_path,
                 nc_strerror(status)));

    if ((status = nc_inq_varid(meta->ncid, netcdf_config->variable_name,
                               &meta->varid)) != NC_NOERR)
      ERR(printf("Cannot find weather variable %s in file %s: %s.",
                 netcdf_config->variable_name, netcdf_config->file_path,
                 nc_strerror(status)));

    if ((status = nc_inq_dimid(meta->ncid, netcdf_config->time_name,
                               &meta->time_dimid)) != NC_NOERR)
      ERR(printf("Cannot find time dimension %s in file %s: %s.",
                 netcdf_config->time_name, netcdf_config->file_path,
                 nc_strerror(status)));

    if ((status = nc_inq_dimlen(meta->ncid, meta->time_dimid,
                                &meta->time_len)) != NC_NOERR)
      ERR(printf("Cannot read time dimension length in file %s: %s.",
                 netcdf_config->file_path, nc_strerror(status)));

    if (meta->time_len <= 0)
      ERR(printf("Time dimension length in file %s is not positive.",
                 netcdf_config->file_path));

    meta->time = malloc(meta->time_len * sizeof(*meta->time));
    if (meta->time == NULL)
      ERR(printf(
          "Could not allocate memory for time values in meta for file %s.",
          netcdf_config->file_path));

    if ((status = nc_inq_varid(meta->ncid, netcdf_config->time_name,
                               &meta->time_varid)) != NC_NOERR)
      ERR(printf("Cannot find time variable %s in file %s: %s.",
                 netcdf_config->time_name, netcdf_config->file_path,
                 nc_strerror(status)));

    size_t time_unit_len;
    if ((status = nc_inq_attlen(meta->ncid, meta->time_varid, "units",
                                &time_unit_len)) != NC_NOERR)
      ERR(printf("Cannot read time variable units length in file %s: %s.",
                 netcdf_config->file_path, nc_strerror(status)));

    if (time_unit_len >= MAX_STRING)
      ERR(printf("Time variable units length exceeds maximum in file %s.",
                 netcdf_config->file_path));

    if ((status = nc_get_att_text(meta->ncid, meta->time_varid, "units",
                                  meta->time_unit)) != NC_NOERR)
      ERR(printf("Cannot read time variable units in file %s: %s.",
                 netcdf_config->file_path, nc_strerror(status)));
    meta->time_unit[time_unit_len] = '\0'; // Null-terminate the string

    int ref_year, ref_month, ref_day;
    char unit[MAX_STRING];
    if (sscanf(meta->time_unit, "%s since %d-%d-%d", unit, &ref_year,
               &ref_month, &ref_day) != 4)
      ERR(printf("Invalid time variable units format in file %s: %s.",
                 netcdf_config->file_path, meta->time_unit));

    struct tm ref_time = {0};
    ref_time.tm_year = ref_year - 1900; // Adjust year for struct tm
    ref_time.tm_mon = ref_month - 1;    // Adjust month for struct tm
    ref_time.tm_mday = ref_day;

    int seconds_per_unit;
    if (strcmp(unit, "days") == 0)
      seconds_per_unit = 86400;
    else if (strcmp(unit, "hours") == 0)
      seconds_per_unit = 3600;
    else if (strcmp(unit, "seconds") == 0)
      seconds_per_unit = 1;
    else
      ERR(printf("Unsupported time unit in file %s: %s. Supported units are "
                 "'days', 'hours', and 'seconds'.",
                 netcdf_config->file_path, unit));

    double *data = malloc(meta->time_len * sizeof(*data));
    if (data == NULL)
      ERR(printf("Could not allocate memory for time values in file %s.",
                 netcdf_config->file_path));

    if ((status = nc_get_var_double(meta->ncid, meta->time_varid, data)) !=
        NC_NOERR)
      ERR(printf("Cannot read time variable values in file %s: %s.",
                 netcdf_config->file_path, nc_strerror(status)));

    for (size_t j = 0; j < meta->time_len; j++) {
      meta->time[j] =
          timegm_portable(&ref_time) + (time_t)(data[j] * seconds_per_unit);
      if (j > 0) {
        if (meta->time[j] - meta->time[j - 1] != TIME_STEP)
          ERR(printf("Time variable in file %s is not in line with model time "
                     "step. Difference between time steps is %ld seconds.",
                     netcdf_config->file_path,
                     meta->time[j] - meta->time[j - 1]));
        if (meta->time[j] <= meta->time[j - 1])
          ERR(printf("Time variable in file %s is not strictly increasing.",
                     netcdf_config->file_path));
      }
    }

    free(data);
  }
}

static void ReadMeteo(const size_t size,
                      const size_t shape[NR_DOMAIN_DIMENSIONS],
                      const NetCDFMeta weather_meta[WEATHER_NTYPES],
                      const time_t current, float data[], DomUnit grid[]) {
  int status;

  size_t start[NR_DOMAIN_DIMENSIONS + 1] = {0, 0, 0};
  size_t count[NR_DOMAIN_DIMENSIONS + 1] = {1, shape[0], shape[1]};

  for (size_t i = 0; i < WEATHER_NTYPES; i++) {
    NetCDFMeta *meta = &weather_meta[i];

    start[0] = current - meta->time[0];

    if ((status = nc_get_vara_float(meta->ncid, meta->varid, start, count,
                                    &data[0])) != NC_NOERR)
      ERR(printf("Cannot read weather variable values: %s.",
                 nc_strerror(status)));

    for (size_t j = 0; j < size; j++) {
      DomUnit *unit = &grid[j];
      Weather *met = &unit->met;

      switch (i) {
      case WEATHER_TMIN:
        met->Tmin = data[j];
        break;
      case WEATHER_TMAX:
        met->Tmax = data[j];
        break;
      case WEATHER_RADIATION:
        met->Radiation = data[j];
        break;
      case WEATHER_RAIN:
        met->Rain = data[j];
        break;
      case WEATHER_WINDSPEED:
        met->Windspeed = data[j];
        break;
      case WEATHER_VAPOUR:
        met->Vapour = data[j];
        break;
      default:
        ERR(printf("Unknown weather variable type %d.", i));
      }
    }
  }
}

static void StopMeteo(NetCDFMeta weather_meta[WEATHER_NTYPES]) {
  for (int i = 0; i < WEATHER_NTYPES; i++) {
    NetCDFMeta *meta = &weather_meta[i];
    free(meta->time);
    meta->time = NULL;
    nc_close(meta->ncid);
  }
}

static void FinalizeMeteoData(float *data) {
  free(data);
  data = NULL;
}

void InitializeMeteo() {
  InitializeMeteoData(Configuration, DomainSize, DomainShape, &WeatherData);
  StartMeteo(Configuration, DomainSize, DomainShape, WeatherMetas);
}

void UpdateMeteo() {
  ReadMeteo(DomainSize, DomainShape, WeatherMetas, CurrentTime, WeatherData,
            DomGrid);
}

void FinalizeMeteo() {
  StopMeteo(WeatherMetas);
  FinalizeMeteoData(WeatherData);
}