#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "defs.h"
#include <stdbool.h>

#define START_DATE_OPTION "START_DATE"
#define END_DATE_OPTION "END_DATE"
#define BMI_COUPLING_OPTION "BMI_COUPLING"
#define OUTPUT_DIRECTORY_OPTION "OUTPUT_DIRECTORY"
#define WEATHER_FILE_OPTION "WEATHER_FILE"

#define CROP_NAME_OPTION "CROP_NAME"
#define EMERGENCE_OPTION "EMERGENCE"
#define CROP_FILE_OPTION "CROP_FILE"
#define MANAGEMENT_FILE_OPTION "MANAGEMENT_FILE"
#define SOIL_FILE_OPTION "SOIL_FILE"
#define SITE_FILE_OPTION "SITE_FILE"
#define DOMAIN_FILE_OPTION "DOMAIN_FILE"

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

static const char WEATHER_VARIABLES[WEATHER_NTYPES][MAX_STRING] = {
    "TMIN", "TMAX", "RADIATION", "RAIN", "WINDSPEED", "VAPOUR"};
static const char DOMAIN_VARIABLES[DOMAIN_NTYPES][MAX_STRING] = {"MASK", "PLANT_DATE",
                                                          "TSUM1", "TSUM2"};
static const char OUTPUT_VARIABLES[OUTPUT_NTYPES][MAX_STRING] = {
    "GROWTH_DAY",        "DEVELOPMENT",       "ROOT_BIOMASS",
    "LEAVES_BIOMASS",    "STEMS_BIOMASS",     "STORAGE_BIOMASS",
    "ROOT_DEPTH",        "LEAF_AREA_INDEX",   "STRESS",
    "WATER_STRESS",      "HEAT_STRESS",       "NUTRIENT_STRESS",
    "ROOT_DEAD",         "LEAVES_DEAD",       "STEMS_DEAD",
    "ROOT_N_CONTENT",    "LEAVES_N_CONTENT",  "STEMS_N_CONTENT",
    "STORAGE_N_CONTENT", "ROOT_P_CONTENT",    "LEAVES_P_CONTENT",
    "STEMS_P_CONTENT",   "STORAGE_P_CONTENT", "ROOT_K_CONTENT",
    "LEAVES_K_CONTENT",  "STEMS_K_CONTENT",   "STORAGE_K_CONTENT"};

typedef struct NETCDFCONFIG {
  char file_path[MAX_STRING];
  char time_name[MAX_STRING];
  char latitude_name[MAX_STRING];
  char longitude_name[MAX_STRING];
  char variable_name[MAX_STRING];
} NetCDFConfig;

typedef struct CROPCONFIG {
  int emergence;
  char crop_name[MAX_STRING];
  char crop_file[MAX_STRING];
  char management_file[MAX_STRING];
  char soil_file[MAX_STRING];
  char site_file[MAX_STRING];
  NetCDFConfig domain_files[DOMAIN_NTYPES];
} CropConfig;

typedef struct CONFIG {
  struct tm Start;
  struct tm End;
  char output_directory[MAX_STRING];
  bool standalone;
  NetCDFConfig weather_files[WEATHER_NTYPES];
  size_t CropSize;
  CropConfig *crop_configurations;
} Config;

typedef struct NETCDFMETA {
  int ncid;
  int time_dimid;
  int lat_dimid;
  int lon_dimid;
  size_t time_len;
  int time_varid;
  size_t lat_len;
  int lat_varid;
  size_t lon_len;
  int lon_varid;
  char time_unit[MAX_STRING];
  time_t *time;
  int varid;
} NetCDFMeta;

extern Config *Configuration;
extern NetCDFMeta WeatherMetas[WEATHER_NTYPES];
extern NetCDFMeta **OutputMetas;

extern char *trim(char *str, size_t len);
extern void ReadConfiguration(const char *config_file, Config *configuration);

#endif // CONFIGURATION_H