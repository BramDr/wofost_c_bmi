#include "configuration.h"
#include "time_utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *WEATHER_VARIABLES[WEATHER_NTYPES] = {
    "TMIN", "TMAX", "RADIATION", "RAIN", "WINDSPEED", "VAPOUR"};
const char *VARIABLE_UNITS[UNIT_NTYPES] = {"KELVIN",
                                           "CELSIUS",
                                           "J_PER_M2_PER_DAY",
                                           "J_PER_M2_PER_S",
                                           "W_PER_M2",
                                           "MM_PER_S",
                                           "KG_PER_M2_PER_S",
                                           "CM_PER_DAY",
                                           "M_PER_DAY",
                                           "M_PER_S",
                                           "KPA",
                                           "HPA",
                                           "PA"};
const char *DOMAIN_VARIABLES[DOMAIN_NTYPES] = {"MASK", "PLANT_DATE", "TSUM1",
                                               "TSUM2"};
const char *OUTPUT_VARIABLES[OUTPUT_NTYPES] = {
    "GROWTH_DAY",        "DEVELOPMENT",       "ROOT_BIOMASS",
    "LEAVES_BIOMASS",    "STEMS_BIOMASS",     "STORAGE_BIOMASS",
    "ROOT_DEPTH",        "LEAF_AREA_INDEX",   "STRESS",
    "WATER_STRESS",      "HEAT_STRESS",       "NUTRIENT_STRESS",
    "ROOT_DEAD",         "LEAVES_DEAD",       "STEMS_DEAD",
    "ROOT_N_CONTENT",    "LEAVES_N_CONTENT",  "STEMS_N_CONTENT",
    "STORAGE_N_CONTENT", "ROOT_P_CONTENT",    "LEAVES_P_CONTENT",
    "STEMS_P_CONTENT",   "STORAGE_P_CONTENT", "ROOT_K_CONTENT",
    "LEAVES_K_CONTENT",  "STEMS_K_CONTENT",   "STORAGE_K_CONTENT"};

static bool skip_comment(char *trimmed) {
  return (trimmed[0] == '#' || trimmed[0] == '*' || trimmed[0] == ';' ||
          trimmed[0] == '\0');
}

static void ReadGeneralConfiguration(FILE *fp, Config *config) {

  char line[MAX_STRING], option[MAX_STRING];
  int used;

  // Clear (zero all struct tm fields first: only the date is read from the
  // config file, but timegm_portable() also reads time-of-day and tm_isdst)
  config->Start = (struct tm){0};
  config->End = (struct tm){0};
  config->Start.tm_year = -1;
  config->Start.tm_mon = -1;
  config->Start.tm_mday = -1;
  config->End.tm_year = -1;
  config->End.tm_mon = -1;
  config->End.tm_mday = -1;
  config->standalone = false;
  config->ignore_nutrient_stress = false;

  // Read
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, START_DATE_OPTION) == 0) {
      if (config->Start.tm_year != -1)
        ERR("Duplicate %s line in config file: %s", START_DATE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%d-%d-%d", &config->Start.tm_year,
                 &config->Start.tm_mon, &config->Start.tm_mday) != 3)
        ERR("Invalid %s line in config file: %s", START_DATE_OPTION, trimmed);
      config->Start.tm_year -= 1900; // Adjust year for struct tm
      config->Start.tm_mon -= 1;     // Adjust month for struct tm
    } else if (strcmp(option, END_DATE_OPTION) == 0) {
      if (config->End.tm_year != -1)
        ERR("Duplicate %s line in config file: %s", END_DATE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%d-%d-%d", &config->End.tm_year,
                 &config->End.tm_mon, &config->End.tm_mday) != 3)
        ERR("Invalid %s line in config file: %s", END_DATE_OPTION, trimmed);
      config->End.tm_year -= 1900; // Adjust year for struct tm
      config->End.tm_mon -= 1;     // Adjust month for struct tm
    } else if (strcmp(option, BMI_COUPLING_OPTION) == 0) {
      char coupling_type[MAX_STRING];
      if (sscanf(trimmed + used, "%s", coupling_type) != 1)
        ERR("Invalid %s line in config file: %s", BMI_COUPLING_OPTION, trimmed);
      if (strcmp(coupling_type, "STANDALONE") == 0) {
        config->standalone = true;
      } else if (strcmp(coupling_type, "COUPLED") == 0) {
        config->standalone = false;
      } else {
        ERR("Invalid %s line in config file: %s", BMI_COUPLING_OPTION, trimmed);
      }
    } else if (strcmp(option, IGNORE_NUTRIENT_STRESS_OPTION) == 0) {
      char ignore_type[MAX_STRING];
      if (sscanf(trimmed + used, "%s", ignore_type) != 1)
        ERR("Invalid %s line in config file: %s", IGNORE_NUTRIENT_STRESS_OPTION,
            trimmed);
      if (strcmp(ignore_type, "TRUE") == 0) {
        config->ignore_nutrient_stress = true;
      } else if (strcmp(ignore_type, "FALSE") == 0) {
        config->ignore_nutrient_stress = false;
      } else {
        ERR("Invalid %s line in config file: %s", IGNORE_NUTRIENT_STRESS_OPTION,
            trimmed);
      }
    }
  }

  // Validate
  if (config->Start.tm_year == -1 || config->Start.tm_mon == -1 ||
      config->Start.tm_mday == -1)
    ERR("Start date must be specified in the configuration.");
  if (config->End.tm_year == -1 || config->End.tm_mon == -1 ||
      config->End.tm_mday == -1)
    ERR("End date must be specified in the configuration.");
  if (timegm_portable(&config->Start) > timegm_portable(&config->End))
    ERR("Start date must be before end date in the configuration.");
}

static void ReadOutputConfiguration(FILE *fp, Config *config) {

  char line[MAX_STRING], option[MAX_STRING];
  int used;

  // Clear
  memset(config->output_directory, 0, MAX_STRING);

  // Read
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, OUTPUT_DIRECTORY_OPTION) == 0) {
      if (strlen(config->output_directory) != 0)
        ERR("Duplicate %s line in config file: %s", OUTPUT_DIRECTORY_OPTION,
            trimmed);
      if (sscanf(trimmed + used, "%s", config->output_directory) != 1)
        ERR("Invalid %s line in config file: %s", OUTPUT_DIRECTORY_OPTION,
            trimmed);
    }
  }

  // Validate
  if (strlen(config->output_directory) == 0)
    ERR("%s must be specified in the configuration.", OUTPUT_DIRECTORY_OPTION);
}

static void ReadAreaConfiguration(FILE *fp, Config *config) {

  char line[MAX_STRING], option[MAX_STRING];
  int used;

  // Clear
  memset(&config->area_file, 0, sizeof(NetCDFConfig));

  // Read
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, AREA_FILE_OPTION) == 0) {
      NetCDFConfig *netcdf_config = &config->area_file;
      if (strlen(netcdf_config->file_path) != 0)
        ERR("Duplicate %s line in config file: %s", AREA_FILE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%s %s", netcdf_config->file_path,
                 netcdf_config->variable_name) != 2)
        ERR("Invalid %s line in config file: %s", AREA_FILE_OPTION, trimmed);
    }
  }

  // Validate
  if (strlen(config->area_file.file_path) == 0)
    ERR("%s must be specified in the configuration.", AREA_FILE_OPTION);
}

static void ReadWeatherConfiguration(FILE *fp, Config *config) {

  char line[MAX_STRING], option[MAX_STRING], type[MAX_STRING],
      units[MAX_STRING];
  int used, used2;

  // Clear
  for (int i = 0; i < WEATHER_NTYPES; i++) {
    memset(&config->weather_files[i], 0, sizeof(NetCDFConfig));
    config->weather_units[i] = UNIT_NTYPES;
  }

  // Read
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, WEATHER_FILE_OPTION) == 0) {
      if (sscanf(trimmed + used, "%s %n", type, &used2) != 1)
        ERR("Invalid weather file line in config file: %s", trimmed);
      size_t j;
      for (j = 0; j < WEATHER_NTYPES; j++) {
        if (strcmp(type, WEATHER_VARIABLES[j]) == 0)
          break;
      }
      if (j == WEATHER_NTYPES)
        ERR("Unknown weather type in config file: %s", type);
      NetCDFConfig *netcdf_config = &config->weather_files[j];
      if (strlen(netcdf_config->file_path) != 0)
        ERR("Duplicate weather file line for type %s in config file: %s",
            WEATHER_VARIABLES[j], trimmed);
      if (sscanf(trimmed + used + used2, "%s %s %s", netcdf_config->file_path,
                 netcdf_config->variable_name, units) != 3)
        ERR("Invalid weather file line in config file: %s", trimmed);
      size_t k;
      for (k = 0; k < UNIT_NTYPES; k++) {
        if (strcmp(units, VARIABLE_UNITS[k]) == 0)
          break;
      }
      if (k == UNIT_NTYPES)
        ERR("Unknown weather units in config file: %s", units);
      config->weather_units[j] = k;
    }
  }

  // Validate
  for (int i = 0; i < WEATHER_NTYPES; i++) {
    if (strlen(config->weather_files[i].file_path) == 0) {
      ERR("Weather file for type %s is not specified in the configuration.\n",
          WEATHER_VARIABLES[i]);
    }
    if (config->weather_units[i] == UNIT_NTYPES) {
      ERR("Weather unit for type %s is not specified in the configuration.\n",
          WEATHER_VARIABLES[i]);
    }
  }
}

static void ReadCropConfiguration(FILE *fp, CropConfig *config) {

  char line[MAX_STRING], option[MAX_STRING], type[MAX_STRING];
  int used, used2;

  while (fgets(line, sizeof(line), fp)) {
    size_t raw_len = strnlen(line, MAX_STRING);
    char *trimmed = trim(line, raw_len);
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, PLANT_DATE_OPTION) == 0) {
      if (config->plant_date != -1)
        ERR("Duplicate %s line in config file: %s", PLANT_DATE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%d", &config->plant_date) != 1)
        ERR("Invalid %s line in config file: %s", PLANT_DATE_OPTION, trimmed);
      if (config->plant_date < 0)
        ERR("Invalid %s line in config file: %s", PLANT_DATE_OPTION, trimmed);
      if (config->plant_date > 365)
        ERR("Invalid %s line in config file: %s", PLANT_DATE_OPTION, trimmed);
    } else if (strcmp(option, EMERGENCE_OPTION) == 0) {
      if (config->emergence != -1)
        ERR("Duplicate %s line in config file: %s", EMERGENCE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%d", &config->emergence) != 1)
        ERR("Invalid %s line in config file: %s", EMERGENCE_OPTION, trimmed);
      if (config->emergence < 0)
        ERR("Invalid %s line in config file: %s", EMERGENCE_OPTION, trimmed);
      if (config->emergence > 1)
        ERR("Invalid %s line in config file: %s", EMERGENCE_OPTION, trimmed);
    } else if (strcmp(option, CROP_FILE_OPTION) == 0) {
      if (strlen(config->crop_file) != 0)
        ERR("Duplicate %s line in config file: %s", CROP_FILE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%s", config->crop_file) != 1)
        ERR("Invalid %s line in config file: %s", CROP_FILE_OPTION, trimmed);
    } else if (strcmp(option, MANAGEMENT_FILE_OPTION) == 0) {
      if (strlen(config->management_file) != 0)
        ERR("Duplicate %s line in config file: %s", MANAGEMENT_FILE_OPTION,
            trimmed);
      if (sscanf(trimmed + used, "%s", config->management_file) != 1)
        ERR("Invalid %s line in config file: %s", MANAGEMENT_FILE_OPTION,
            trimmed);
    } else if (strcmp(option, SOIL_FILE_OPTION) == 0) {
      if (strlen(config->soil_file) != 0)
        ERR("Duplicate %s line in config file: %s", SOIL_FILE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%s", config->soil_file) != 1)
        ERR("Invalid %s line in config file: %s", SOIL_FILE_OPTION, trimmed);
    } else if (strcmp(option, SITE_FILE_OPTION) == 0) {
      if (strlen(config->site_file) != 0)
        ERR("Duplicate %s line in config file: %s", SITE_FILE_OPTION, trimmed);
      if (sscanf(trimmed + used, "%s", config->site_file) != 1)
        ERR("Invalid %s line in config file: %s", SITE_FILE_OPTION, trimmed);
    } else if (strcmp(option, DOMAIN_FILE_OPTION) == 0) {
      if (sscanf(trimmed + used, "%s %n", type, &used2) != 1)
        ERR("Invalid %s line in config file: %s", DOMAIN_FILE_OPTION, trimmed);
      size_t j;
      for (j = 0; j < DOMAIN_NTYPES; j++) {
        if (strcmp(type, DOMAIN_VARIABLES[j]) == 0)
          break;
      }
      if (j == DOMAIN_NTYPES)
        ERR("Unknown domain type in config file: %s", type);
      NetCDFConfig *netcdf_config = &config->domain_files[j];
      if (strlen(netcdf_config->file_path) != 0)
        ERR("Duplicate domain file line for type %s in config file: %s\n", type,
            trimmed);
      if (sscanf(trimmed + used + used2, "%s %s", netcdf_config->file_path,
                 netcdf_config->variable_name) != 2)
        ERR("Invalid %s line in config file: %s", DOMAIN_FILE_OPTION, trimmed);
    } else if (strcmp(option, OUTPUT_VAR_OPTION) == 0) {
      if (sscanf(trimmed + used, "%s", type) != 1)
        ERR("Invalid %s line in config file: %s", OUTPUT_VAR_OPTION, trimmed);
      size_t j;
      for (j = 0; j < OUTPUT_NTYPES; j++) {
        if (strcmp(type, OUTPUT_VARIABLES[j]) == 0)
          break;
      }
      if (j == OUTPUT_NTYPES)
        ERR("Unknown output variable in config file: %s", type);
      if (config->output_types[j])
        ERR("Duplicate %s line in config file: %s", OUTPUT_VAR_OPTION, trimmed);
      config->output_types[j] = true;
    } else {
      fseek(fp, -(long)raw_len, SEEK_CUR);
      break;
    }
  }
}

static void ReadCropsConfiguration(FILE *fp, Config *config) {

  char line[MAX_STRING], option[MAX_STRING];
  int used;

  // Clear
  if (config->crop_configurations != NULL) {
    free(config->crop_configurations);
    config->crop_configurations = NULL;
  }

  // Allocate
  config->CropSize = 0;
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, CROP_NAME_OPTION) != 0)
      continue;
    config->CropSize++;
  }
  if (config->CropSize == 0)
    ERR("No crop configurations found in config file.");
  config->crop_configurations = malloc(config->CropSize * sizeof(CropConfig));
  if (config->crop_configurations == NULL)
    ERR("Cannot allocate memory for crop configurations.");

  // Clear
  for (size_t i = 0; i < config->CropSize; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];
    crop_config->plant_date = -1;
    crop_config->emergence = -1;
    memset(crop_config->crop_name, 0, MAX_STRING);
    memset(crop_config->crop_file, 0, MAX_STRING);
    memset(crop_config->management_file, 0, MAX_STRING);
    memset(crop_config->soil_file, 0, MAX_STRING);
    memset(crop_config->site_file, 0, MAX_STRING);
    for (int j = 0; j < DOMAIN_NTYPES; j++) {
      memset(&crop_config->domain_files[j], 0, sizeof(NetCDFConfig));
    }
    for (int j = 0; j < OUTPUT_NTYPES; j++) {
      crop_config->output_types[j] = false;
    }
  }

  // Read
  rewind(fp);
  size_t crop_index = 0;
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, CROP_NAME_OPTION) != 0)
      continue;
    if (crop_index >= config->CropSize)
      ERR("More crop configurations found than expected in config file.");

    CropConfig *crop_config = &config->crop_configurations[crop_index];
    if (sscanf(trimmed + used, "%s", crop_config->crop_name) != 1)
      ERR("Invalid %s line in config file: %s", CROP_NAME_OPTION, trimmed);

    ReadCropConfiguration(fp, crop_config);
    crop_index++;
  }

  // Validate
  for (size_t i = 0; i < config->CropSize; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];
    if (strlen(crop_config->crop_name) == 0)
      ERR("Crop name must be specified in the crop configuration.");
    if (crop_config->plant_date == -1)
      ERR("Plant date must be specified in the crop configuration for crop %s.",
          crop_config->crop_name);
    if (crop_config->emergence == -1)
      ERR("Emergence must be specified in the crop configuration for crop %s.",
          crop_config->crop_name);
    if (strlen(crop_config->crop_file) == 0)
      ERR("Crop file must be specified in the crop configuration for crop %s.",
          crop_config->crop_name);
    if (strlen(crop_config->management_file) == 0)
      ERR("Management file must be specified in the crop configuration for "
          "crop %s.",
          crop_config->crop_name);
    if (strlen(crop_config->soil_file) == 0)
      ERR("Soil file must be specified in the crop configuration for crop %s.",
          crop_config->crop_name);
    if (strlen(crop_config->site_file) == 0)
      ERR("Site file must be specified in the crop configuration for crop %s.",
          crop_config->crop_name);
    for (int j = 0; j < DOMAIN_NTYPES; j++) {
      if (j > DOMAIN_PLANT_DATE) {
        continue;
      }
      if (strlen(crop_config->domain_files[j].file_path) == 0) {
        ERR("Domain file for type %s is not specified in the crop "
            "configuration for crop %s.\n",
            DOMAIN_VARIABLES[j], crop_config->crop_name);
      }
    }
    bool has_output_type = false;
    for (int j = 0; j < OUTPUT_NTYPES; j++) {
      if (crop_config->output_types[j]) {
        has_output_type = true;
        break;
      }
    }
    if (!has_output_type) {
      WARN("No output types specified in the crop configuration for crop %s",
           crop_config->crop_name);
    }
    for (size_t j = i + 1; j < config->CropSize; j++) {
      CropConfig *crop_config_other = &config->crop_configurations[j];
      if (strcmp(crop_config->crop_name, crop_config_other->crop_name) == 0) {
        ERR("Duplicate crop name found in crop configurations: %s",
            crop_config->crop_name);
      }
    }
  }
}

void ReadConfiguration(const char *config_file, Config *configuration) {
  DBG("ReadConfiguration");

  FILE *fp = fopen(config_file, "r");
  if (fp == NULL)
    ERR("Cannot open config file %s.", config_file);

  ReadGeneralConfiguration(fp, configuration);
  rewind(fp);
  ReadOutputConfiguration(fp, configuration);
  rewind(fp);
  ReadAreaConfiguration(fp, configuration);
  rewind(fp);
  ReadWeatherConfiguration(fp, configuration);
  rewind(fp);
  ReadCropsConfiguration(fp, configuration);

  fclose(fp);
}
