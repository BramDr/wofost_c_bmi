#include "simulation.h"
#include "time_utils.h"
#include <math.h>
#include <netcdf.h>
#include <string.h>

/* set decimals */
#define roundz(x, d) ((floor(((x) * pow(10, d)) + .5)) / pow(10, d))
#define roundz1(x) ((floor(((x) * 10) + .5)) / 10)
#define roundz2(x) ((floor(((x) * 100) + .5)) / 100)

static void InitializeMeteoData(const size_t size, float **data) {

  *data = malloc(size * sizeof(**data));
  if (*data == NULL)
    ERR("Could not allocate memory for weather data.");
}

static void StartMeteo(const Config *config, const double west,
                       const double east, const double south,
                       const double north, const size_t start, const size_t end,
                       const size_t shape[NR_DOMAIN_DIMENSIONS],
                       NetCDFMeta metas[WEATHER_NTYPES]) {

  for (int i = 0; i < WEATHER_NTYPES; i++) {
    const NetCDFConfig *netcdf_config = &config->weather_files[i];
    const size_t var_unit = config->weather_units[i];
    NetCDFMeta *meta = &metas[i];

    InitializeNetCDFMeta(netcdf_config->file_path, netcdf_config->variable_name,
                         meta);
    DeriveNetCDFMetaSpace(west, east, south, north, shape, meta);
    DeriveNetCDFMetaTime(start, end, meta);
    meta->var_unit = var_unit;
  }
}

static void ReadMeteo(const size_t size, const NetCDFMeta metas[WEATHER_NTYPES],
                      const time_t current, float data[], DomUnit grid[]) {

  for (size_t i = 0; i < WEATHER_NTYPES; i++) {
    const NetCDFMeta *meta = &metas[i];
    size_t var_unit = meta->var_unit;

    size_t time_start = (size_t)((current - meta->time[0]) / (time_t)TIME_STEP);
    ReadNetCDFMetaFloat(meta, NAN, data, time_start);

    for (size_t j = 0; j < size; j++) {
      DomUnit *unit = &grid[j];
      Weather *met = &unit->met;
      float element = data[j];

      switch (i) {
      case WEATHER_TMIN:
        switch (var_unit) {
        case UNIT_CELSIUS:
          met->Tmin = roundz1(element);
          break;
        case UNIT_KELVIN:
          met->Tmin = roundz1(element - 273.15);
          break;
        default:
          ERR("Unknown weather variable unit %zu for TMIN.", var_unit);
        }
        break;
      case WEATHER_TMAX:
        switch (var_unit) {
        case UNIT_CELSIUS:
          met->Tmax = roundz1(element);
          break;
        case UNIT_KELVIN:
          met->Tmax = roundz1(element - 273.15);
          break;
        default:
          ERR("Unknown weather variable unit %zu for TMAX.", var_unit);
        }
        break;
      case WEATHER_RADIATION:
        switch (var_unit) {
        case UNIT_J_PER_M2_PER_DAY:
          met->Radiation = roundz1(element);
          break;
        case UNIT_J_PER_M2_PER_S:
        case UNIT_W_PER_M2:
          met->Radiation = roundz1(86400 * element);
          break;
        default:
          ERR("Unknown weather variable unit %zu for RADIATION.", var_unit);
        }
        break;
      case WEATHER_RAIN:
        switch (var_unit) {
        case UNIT_CM_PER_DAY:
          met->Rain = roundz2(element);
          break;
        case UNIT_MM_PER_S:
        case UNIT_KG_PER_M2_PER_S:
          met->Rain = roundz2(0.1 * 86400 * element);
          break;
        case UNIT_M_PER_DAY:
          met->Rain = roundz2(100 * element);
          break;
        default:
          ERR("Unknown weather variable unit %zu for RAIN.", var_unit);
        }
        break;
      case WEATHER_WINDSPEED:
        switch (var_unit) {
        case UNIT_M_PER_S:
          met->Windspeed = roundz1(element);
          break;
        default:
          ERR("Unknown weather variable unit %zu for WINDSPEED.", var_unit);
        }
        break;
      case WEATHER_VAPOUR:
        switch (var_unit) {
        case UNIT_HPA:
          met->Vapour = roundz1(element);
          break;
        case UNIT_KPA:
          met->Vapour = roundz1(10 * element);
          break;
        default:
          ERR("Unknown weather variable unit %zu for VAPOUR.", var_unit);
        }
        break;
      default:
        ERR("Unknown weather variable type %zu.", i);
      }
    }
  }
}

static void StopMeteo(NetCDFMeta metas[WEATHER_NTYPES]) {

  for (int i = 0; i < WEATHER_NTYPES; i++) {
    NetCDFMeta *meta = &metas[i];
    FreeNetCDFMeta(meta);
  }
}

static void FinalizeMeteoData(float *data) {

  free(data);
  data = NULL;
}

void InitializeMeteo(void) {
  DBG("InitializeMeteo");

  InitializeMeteoData(DomainSize, &WeatherData);

  time_t StartTime = timegm_portable(&Start);
  time_t EndTime = timegm_portable(&End);
  StartMeteo(Configuration, West, East, South, North, StartTime, EndTime,
             DomainShape, WeatherMetas);
}

void UpdateMeteo(void) {
  DBG("UpdateMeteo");

  ReadMeteo(DomainSize, WeatherMetas, CurrentTime, WeatherData, DomGrid);
}

void FinalizeMeteo(void) {
  DBG("FinalizeMeteo");

  StopMeteo(WeatherMetas);
  FinalizeMeteoData(WeatherData);
}