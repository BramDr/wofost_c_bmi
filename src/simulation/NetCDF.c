#include "simulation.h"
#include "time_utils.h"
#include <math.h>
#include <netcdf.h>
#include <string.h>

void InitializeNetCDFMeta(const char *path, const char *variable_name,
                          NetCDFMeta *meta) {
  int status;

  meta->path = NULL;
  meta->ncid = -1;
  meta->varid = -1;
  meta->lat_dimid = -1;
  meta->lat_len = 0;
  meta->lat_varid = -1;
  meta->lat = NULL;
  meta->lon_dimid = -1;
  meta->lon_len = 0;
  meta->lon_varid = -1;
  meta->lon = NULL;
  meta->time_dimid = -1;
  meta->time_len = 0;
  meta->time_varid = -1;
  meta->time = NULL;

  meta->path = strdup(path);
  if (meta->path == NULL)
    ERR("Could not allocate memory for NetCDFMeta path.");

  if ((status = nc_open(meta->path, NC_NOWRITE, &meta->ncid)) != NC_NOERR)
    ERR("Cannot open file %s: %s.", meta->path, nc_strerror(status));

  // Derive variable
  if (strlen(variable_name) > 0) {
    if ((status = nc_inq_varid(meta->ncid, variable_name, &meta->varid)) !=
        NC_NOERR)
      ERR("Cannot find variable %s in file %s: %s.", variable_name, meta->path,
          nc_strerror(status));
  }

  // Derive latitude and longitude dimensions, variables and data
  if ((status = nc_inq_dimid(meta->ncid, "lat", &meta->lat_dimid)) != NC_NOERR)
    ERR("Cannot find latitude dimension 'lat' in file %s: %s.", meta->path,
        nc_strerror(status));
  if ((status = nc_inq_dimid(meta->ncid, "lon", &meta->lon_dimid)) != NC_NOERR)
    ERR("Cannot find longitude dimension 'lon' in file %s: %s.", meta->path,
        nc_strerror(status));

  if ((status = nc_inq_dimlen(meta->ncid, meta->lat_dimid, &meta->lat_len)) !=
      NC_NOERR)
    ERR("Cannot read latitude dimension length: %s.", nc_strerror(status));
  if ((status = nc_inq_dimlen(meta->ncid, meta->lon_dimid, &meta->lon_len)) !=
      NC_NOERR)
    ERR("Cannot read longitude dimension length: %s.", nc_strerror(status));

  if (meta->lat_len <= 0)
    ERR("Latitude dimension length in file %s is not positive.", meta->path);
  if (meta->lon_len <= 0)
    ERR("Longitude dimension length in file %s is not positive.", meta->path);

  if ((status = nc_inq_varid(meta->ncid, "lat", &meta->lat_varid)) != NC_NOERR)
    ERR("Cannot find latitude variable 'lat' in file %s: %s.", meta->path,
        nc_strerror(status));
  if ((status = nc_inq_varid(meta->ncid, "lon", &meta->lon_varid)) != NC_NOERR)
    ERR("Cannot find longitude variable 'lon' in file %s: %s.", meta->path,
        nc_strerror(status));

  meta->lat = malloc(meta->lat_len * sizeof(*meta->lat));
  if (meta->lat == NULL)
    ERR("Could not allocate memory for Latitudes.");
  meta->lon = malloc(meta->lon_len * sizeof(*meta->lon));
  if (meta->lon == NULL)
    ERR("Could not allocate memory for Longitudes.");

  if ((status = nc_get_var_double(meta->ncid, meta->lat_varid, meta->lat)) !=
      NC_NOERR)
    ERR("Cannot read latitude variable values: %s.", nc_strerror(status));
  if ((status = nc_get_var_double(meta->ncid, meta->lon_varid, meta->lon)) !=
      NC_NOERR)
    ERR("Cannot read longitude variable values: %s.", nc_strerror(status));

  // If latitudes and/or longitudes are stored largest-to-smallest, flip them
  // to ascending order in memory. The original on-disk order is preserved by
  // lat_flipped/lon_flipped so that ReadNetCDFMetaFloat/ReadNetCDFMetaInt can
  // translate ascending-order indices back to file indices when reading data.
  meta->lat_flipped =
      meta->lat_len > 1 && meta->lat[0] > meta->lat[meta->lat_len - 1];
  if (meta->lat_flipped) {
    for (size_t i = 0; i < meta->lat_len / 2; i++) {
      double tmp = meta->lat[i];
      meta->lat[i] = meta->lat[meta->lat_len - 1 - i];
      meta->lat[meta->lat_len - 1 - i] = tmp;
    }
  }

  meta->lon_flipped =
      meta->lon_len > 1 && meta->lon[0] > meta->lon[meta->lon_len - 1];
  if (meta->lon_flipped) {
    for (size_t i = 0; i < meta->lon_len / 2; i++) {
      double tmp = meta->lon[i];
      meta->lon[i] = meta->lon[meta->lon_len - 1 - i];
      meta->lon[meta->lon_len - 1 - i] = tmp;
    }
  }

  // Derive time dimension, variable and data
  status = nc_inq_dimid(meta->ncid, "time", &meta->time_dimid);
  if (status != NC_EBADDIM) {

    if (status != NC_NOERR)
      ERR("Cannot find time dimension 'time' in file %s: %s.", meta->path,
          nc_strerror(status));

    if ((status = nc_inq_dimlen(meta->ncid, meta->time_dimid,
                                &meta->time_len)) != NC_NOERR)
      ERR("Cannot read time dimension length in file %s: %s.", meta->path,
          nc_strerror(status));

    if (meta->time_len <= 0)
      ERR("Time dimension length in file %s is not positive.", meta->path);

    meta->time = malloc(meta->time_len * sizeof(*meta->time));
    if (meta->time == NULL)
      ERR("Could not allocate memory for time values in meta for file %s.",
          meta->path);

    if ((status = nc_inq_varid(meta->ncid, "time", &meta->time_varid)) !=
        NC_NOERR)
      ERR("Cannot find time variable 'time' in file %s: %s.", meta->path,
          nc_strerror(status));

    size_t time_unit_len;
    char time_unit[MAX_STRING];
    if ((status = nc_inq_attlen(meta->ncid, meta->time_varid, "units",
                                &time_unit_len)) != NC_NOERR)
      ERR("Cannot read time variable units length in file %s: %s.", meta->path,
          nc_strerror(status));

    if (time_unit_len >= MAX_STRING)
      ERR("Time variable units length exceeds maximum in file %s.", meta->path);

    if ((status = nc_get_att_text(meta->ncid, meta->time_varid, "units",
                                  time_unit)) != NC_NOERR)
      ERR("Cannot read time variable units in file %s: %s.", meta->path,
          nc_strerror(status));
    time_unit[time_unit_len] = '\0'; // Null-terminate the string

    int ref_year, ref_month, ref_day;
    char unit[MAX_STRING];
    if (sscanf(time_unit, "%s since %d-%d-%d", unit, &ref_year, &ref_month,
               &ref_day) != 4)
      ERR("Invalid time variable units format in file %s: %s.", meta->path,
          time_unit);

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
      ERR("Unsupported time unit in file %s: %s. Supported units are "
          "'days', 'hours', and 'seconds'.",
          meta->path, unit);

    double *data = malloc(meta->time_len * sizeof(*data));
    if (data == NULL)
      ERR("Could not allocate memory for time values in file %s.", meta->path);

    if ((status = nc_get_var_double(meta->ncid, meta->time_varid, data)) !=
        NC_NOERR)
      ERR("Cannot read time variable values in file %s: %s.", meta->path,
          nc_strerror(status));

    for (size_t j = 0; j < meta->time_len; j++) {
      meta->time[j] =
          timegm_portable(&ref_time) + (time_t)(data[j] * seconds_per_unit);
      if (j > 0) {
        if (meta->time[j] - meta->time[j - 1] != TIME_STEP)
          ERR("Time variable in file %s is not in line with model time "
              "step. Difference between time steps is %ld seconds.",
              meta->path, meta->time[j] - meta->time[j - 1]);
        if (meta->time[j] <= meta->time[j - 1])
          ERR("Time variable in file %s is not strictly increasing.",
              meta->path);
      }
    }
  }
}

void DeriveNetCDFMetaSpace(const double west, const double east,
                           const double south, const double north,
                           const size_t shape[NR_DOMAIN_DIMENSIONS],
                           NetCDFMeta *meta) {

  meta->lat_start = 0;
  meta->lat_count = 0;
  meta->lon_start = 0;
  meta->lon_count = 0;

  while (meta->lat_start < meta->lat_len &&
         meta->lat[meta->lat_start] < south) {
    meta->lat_start++;
  }

  if (meta->lat_start >= meta->lat_len)
    ERR("South latitude %lf is beyond the last latitude in NetCDF file.",
        south);

  while (meta->lat_start + meta->lat_count < meta->lat_len &&
         meta->lat[meta->lat_start + meta->lat_count] <= north) {
    meta->lat_count++;
  }

  while (meta->lon_start < meta->lon_len && meta->lon[meta->lon_start] < west) {
    meta->lon_start++;
  }

  if (meta->lon_start >= meta->lon_len)
    ERR("West longitude %lf is beyond the last longitude in NetCDF file.",
        west);

  while (meta->lon_start + meta->lon_count < meta->lon_len &&
         meta->lon[meta->lon_start + meta->lon_count] <= east) {
    meta->lon_count++;
  }

  if (shape[0] != meta->lat_count || shape[1] != meta->lon_count)
    ERR("Mismatch between domain shape and NetCDF file. Domain shape: "
        "[%zu, %zu], NetCDF shape: [%zu, %zu].",
        shape[0], shape[1], meta->lat_count, meta->lon_count);
}

void DeriveNetCDFMetaTime(const time_t start, const time_t end,
                          NetCDFMeta *meta) {

  meta->time_start = 0;
  meta->time_count = 0;

  while (meta->time_start < meta->time_len &&
         meta->time[meta->time_start] < start) {
    meta->time_start++;
  }

  if (meta->time_start >= meta->time_len)
    ERR("Start time %ld is beyond the last time step in NetCDF file.", start);

  if (meta->time[meta->time_start] != start)
    ERR("Start time %ld does not match any time step in NetCDF file.", start);

  while (meta->time_start + meta->time_count < meta->time_len &&
         meta->time[meta->time_start + meta->time_count] <= end) {
    meta->time_count++;
  }

  if (meta->time_start + meta->time_count > meta->time_len)
    ERR("End time %ld is beyond the last time step in NetCDF file.", end);
}

// Flips a 2D [lat, lon] slab in place along the lat and/or lon axes. Used to
// bring data read from a file with descending lat/lon order in line with the
// ascending order exposed via NetCDFMeta.lat/lon.
static void FlipDataFloat(float data[], size_t lat_count, size_t lon_count,
                          bool flip_lat, bool flip_lon) {
  if (flip_lat) {
    for (size_t i = 0; i < lat_count / 2; i++) {
      size_t j = lat_count - 1 - i;
      for (size_t k = 0; k < lon_count; k++) {
        float tmp = data[i * lon_count + k];
        data[i * lon_count + k] = data[j * lon_count + k];
        data[j * lon_count + k] = tmp;
      }
    }
  }

  if (flip_lon) {
    for (size_t i = 0; i < lat_count; i++) {
      for (size_t k = 0; k < lon_count / 2; k++) {
        size_t l = lon_count - 1 - k;
        float tmp = data[i * lon_count + k];
        data[i * lon_count + k] = data[i * lon_count + l];
        data[i * lon_count + l] = tmp;
      }
    }
  }
}

void ReadNetCDFMetaFloat(const NetCDFMeta *meta, const float fill_value,
                         float data[], size_t time_index) {
  int status;
  float var_fill;
  size_t start[NR_DOMAIN_DIMENSIONS + 1];
  size_t count[NR_DOMAIN_DIMENSIONS + 1];

  // meta->lat_start/lon_start are indices into the ascending-order lat/lon
  // arrays. If the file itself stores lat/lon descending, translate to the
  // corresponding file-order start index before reading.
  size_t lat_start = meta->lat_flipped
                         ? meta->lat_len - meta->lat_start - meta->lat_count
                         : meta->lat_start;
  size_t lon_start = meta->lon_flipped
                         ? meta->lon_len - meta->lon_start - meta->lon_count
                         : meta->lon_start;

  if (meta->time_dimid >= 0) {
    start[0] = time_index;
    start[1] = lat_start;
    start[2] = lon_start;
    count[0] = 1;
    count[1] = meta->lat_count;
    count[2] = meta->lon_count;
  } else {
    start[0] = lat_start;
    start[1] = lon_start;
    count[0] = meta->lat_count;
    count[1] = meta->lon_count;
  }

  if ((status = nc_get_vara_float(meta->ncid, meta->varid, start, count,
                                  data)) != NC_NOERR)
    ERR("Cannot read variable values from file %s: %s.", meta->path,
        nc_strerror(status));

  if (meta->lat_flipped || meta->lon_flipped)
    FlipDataFloat(data, meta->lat_count, meta->lon_count, meta->lat_flipped,
                  meta->lon_flipped);

  if ((status = nc_inq_var_fill(meta->ncid, meta->varid, NULL, &var_fill)) !=
      NC_NOERR)
    ERR("Cannot query fill value for variable %s: %s.", meta->path,
        nc_strerror(status));

  for (size_t i = 0; i < meta->lat_count * meta->lon_count; i++) {
    if (data[i] == var_fill || isnan(data[i]))
      data[i] = fill_value;
  }
}

// Int counterpart of FlipDataFloat; see that function for details.
static void FlipDataInt(int data[], size_t lat_count, size_t lon_count,
                        bool flip_lat, bool flip_lon) {
  if (flip_lat) {
    for (size_t i = 0; i < lat_count / 2; i++) {
      size_t j = lat_count - 1 - i;
      for (size_t k = 0; k < lon_count; k++) {
        int tmp = data[i * lon_count + k];
        data[i * lon_count + k] = data[j * lon_count + k];
        data[j * lon_count + k] = tmp;
      }
    }
  }

  if (flip_lon) {
    for (size_t i = 0; i < lat_count; i++) {
      for (size_t k = 0; k < lon_count / 2; k++) {
        size_t l = lon_count - 1 - k;
        int tmp = data[i * lon_count + k];
        data[i * lon_count + k] = data[i * lon_count + l];
        data[i * lon_count + l] = tmp;
      }
    }
  }
}

void ReadNetCDFMetaInt(const NetCDFMeta *meta, const int fill_value, int data[],
                       size_t time_index) {
  int status;
  int var_fill;
  size_t start[NR_DOMAIN_DIMENSIONS + 1];
  size_t count[NR_DOMAIN_DIMENSIONS + 1];

  size_t lat_start = meta->lat_flipped
                         ? meta->lat_len - meta->lat_start - meta->lat_count
                         : meta->lat_start;
  size_t lon_start = meta->lon_flipped
                         ? meta->lon_len - meta->lon_start - meta->lon_count
                         : meta->lon_start;

  if (meta->time_dimid >= 0) {
    start[0] = time_index;
    start[1] = lat_start;
    start[2] = lon_start;
    count[0] = 1;
    count[1] = meta->lat_count;
    count[2] = meta->lon_count;
  } else {
    start[0] = lat_start;
    start[1] = lon_start;
    count[0] = meta->lat_count;
    count[1] = meta->lon_count;
  }

  if ((status = nc_get_vara_int(meta->ncid, meta->varid, start, count, data)) !=
      NC_NOERR)
    ERR("Cannot read variable values in file %s: %s.", meta->path,
        nc_strerror(status));

  if (meta->lat_flipped || meta->lon_flipped)
    FlipDataInt(data, meta->lat_count, meta->lon_count, meta->lat_flipped,
                meta->lon_flipped);

  if ((status = nc_inq_var_fill(meta->ncid, meta->varid, NULL, &var_fill)) !=
      NC_NOERR)
    ERR("Cannot query fill value for variable %s: %s.", meta->path,
        nc_strerror(status));

  for (size_t i = 0; i < meta->lat_count * meta->lon_count; i++) {
    if (data[i] == var_fill)
      data[i] = fill_value;
  }
}

void FreeNetCDFMeta(NetCDFMeta *meta) {
  int status;

  if (meta == NULL)
    return;

  if ((status = nc_close(meta->ncid)) != NC_NOERR)
    ERR("Cannot close file %s: %s.", meta->path, nc_strerror(status));
  meta->ncid = -1; // Reset ncid to indicate that the file is closed

  free(meta->path);
  meta->path = NULL;

  free(meta->lat);
  meta->lat = NULL;

  free(meta->lon);
  meta->lon = NULL;

  free(meta->time);
  meta->time = NULL;
}