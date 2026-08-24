#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "defs.h"
#include <stdbool.h>

#define START_DATE_OPTION "START_DATE"
#define END_DATE_OPTION "END_DATE"
#define BMI_COUPLING_OPTION "BMI_COUPLING"
#define AREA_FILE_OPTION "AREA_FILE"
#define OUTPUT_DIRECTORY_OPTION "OUTPUT_DIRECTORY"
#define WEATHER_FILE_OPTION "WEATHER_FILE"
#define IGNORE_NUTRIENT_STRESS_OPTION "IGNORE_NUTRIENT_STRESS"

#define CROP_NAME_OPTION "CROP_NAME"
#define PLANT_DATE_OPTION "PLANT_DATE"
#define EMERGENCE_OPTION "EMERGENCE"
#define CROP_FILE_OPTION "CROP_FILE"
#define MANAGEMENT_FILE_OPTION "MANAGEMENT_FILE"
#define SOIL_FILE_OPTION "SOIL_FILE"
#define SITE_FILE_OPTION "SITE_FILE"
#define DOMAIN_FILE_OPTION "DOMAIN_FILE"
#define OUTPUT_VAR_OPTION "OUTPUT_TYPE"

enum {
  WEATHER_TMIN,
  WEATHER_TMAX,
  WEATHER_RADIATION,
  WEATHER_RAIN,
  WEATHER_WINDSPEED,
  WEATHER_VAPOUR,
  WEATHER_NTYPES
};

enum {
  // Temperature
  UNIT_KELVIN,
  UNIT_CELSIUS,
  // Energy
  UNIT_J_PER_M2_PER_DAY,
  UNIT_J_PER_M2_PER_S,
  UNIT_W_PER_M2,
  // Mass
  UNIT_MM_PER_S,
  UNIT_KG_PER_M2_PER_S,
  UNIT_CM_PER_DAY,
  UNIT_M_PER_DAY,
  // Speed
  UNIT_M_PER_S,
  // Pressure
  UNIT_KPA,
  UNIT_HPA,
  UNIT_PA,
  UNIT_NTYPES
};

enum {
  DOMAIN_MASK,
  DOMAIN_PLANT_DATE,
  DOMAIN_TSUM1,
  DOMAIN_TSUM2,
  DOMAIN_NTYPES
};

enum {
  OUTPUT_GROWTH_DAY,
  OUTPUT_DEVELOPMENT,
  OUTPUT_ROOT_BIOMASS,
  OUTPUT_LEAVES_BIOMASS,
  OUTPUT_STEMS_BIOMASS,
  OUTPUT_STORAGE_BIOMASS,
  OUTPUT_ROOT_DEPTH,
  OUTPUT_LEAF_AREA_INDEX,
  OUTPUT_STRESS,
  OUTPUT_WATER_STRESS,
  OUTPUT_HEAT_STRESS,
  OUTPUT_NUTRIENT_STRESS,
  OUTPUT_ROOT_DEAD,
  OUTPUT_LEAVES_DEAD,
  OUTPUT_STEMS_DEAD,
  OUTPUT_ROOT_N_CONTENT,
  OUTPUT_LEAVES_N_CONTENT,
  OUTPUT_STEMS_N_CONTENT,
  OUTPUT_STORAGE_N_CONTENT,
  OUTPUT_ROOT_P_CONTENT,
  OUTPUT_LEAVES_P_CONTENT,
  OUTPUT_STEMS_P_CONTENT,
  OUTPUT_STORAGE_P_CONTENT,
  OUTPUT_ROOT_K_CONTENT,
  OUTPUT_LEAVES_K_CONTENT,
  OUTPUT_STEMS_K_CONTENT,
  OUTPUT_STORAGE_K_CONTENT,
  OUTPUT_NTYPES,
};

typedef struct NETCDFCONFIG {
  char file_path[MAX_STRING];
  char time_name[MAX_STRING];
  char latitude_name[MAX_STRING];
  char longitude_name[MAX_STRING];
  char variable_name[MAX_STRING];
} NetCDFConfig;

typedef struct CROPCONFIG {
  int plant_date;
  int emergence;
  char crop_name[MAX_STRING];
  char crop_file[MAX_STRING];
  char management_file[MAX_STRING];
  char soil_file[MAX_STRING];
  char site_file[MAX_STRING];
  NetCDFConfig domain_files[DOMAIN_NTYPES];
  bool output_types[OUTPUT_NTYPES];
} CropConfig;

typedef struct CONFIG {
  struct tm Start;
  struct tm End;
  char output_directory[MAX_STRING];
  bool standalone;
  bool ignore_nutrient_stress;
  NetCDFConfig area_file;
  NetCDFConfig weather_files[WEATHER_NTYPES];
  size_t weather_units[WEATHER_NTYPES];
  size_t CropSize;
  CropConfig *crop_configurations;
} Config;

typedef struct NETCDFMETA {
  char *path;
  int ncid;

  int lat_dimid;
  size_t lat_len;
  int lat_varid;
  double *lat;
  size_t lat_start;
  size_t lat_count;
  bool lat_flipped;

  int lon_dimid;
  size_t lon_len;
  int lon_varid;
  double *lon;
  size_t lon_start;
  size_t lon_count;
  bool lon_flipped;

  int time_dimid;
  size_t time_len;
  int time_varid;
  time_t *time;
  size_t time_start;
  size_t time_count;

  int varid;
  size_t var_unit;

} NetCDFMeta;

extern Config *Configuration;
extern NetCDFMeta AreaMeta;
extern NetCDFMeta WeatherMetas[WEATHER_NTYPES];
extern size_t *OutputSizes;
extern size_t **OutputTypes;
extern NetCDFMeta **OutputMetas;

extern char *trim(char *str, size_t len);
extern void ReadConfiguration(const char *config_file, Config *configuration);

#endif // CONFIGURATION_H