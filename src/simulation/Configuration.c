#include "configuration.h"
#include "time_utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool skip_comment(char *trimmed) {
  return (trimmed[0] == '#' || trimmed[0] == '*' || trimmed[0] == ';' ||
          trimmed[0] == '\0');
}

static void ReadGeneralConfiguration(FILE *fp, Config *config) {
  DBG("ReadGeneralConfiguration");

  char line[MAX_STRING], option[MAX_STRING];
  int used;

  // Clear
  config->Start.tm_year = -1;
  config->Start.tm_mon = -1;
  config->Start.tm_mday = -1;
  config->End.tm_year = -1;
  config->End.tm_mon = -1;
  config->End.tm_mday = -1;
  config->standalone = false;

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
  DBG("ReadOutputConfiguration");

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

static void ReadWeatherConfiguration(FILE *fp, Config *config) {
  DBG("ReadWeatherConfiguration");

  char line[MAX_STRING], option[MAX_STRING], type[MAX_STRING];
  int used, used2;

  // Clear
  for (int i = 0; i < WEATHER_NTYPES; i++) {
    memset(&config->weather_files[i], 0, sizeof(NetCDFConfig));
  }

  // Read
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (strcmp(option, WEATHER_FILE_OPTION) != 0)
      continue;

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
    if (sscanf(trimmed + used + used2, "%s %s %s %s %s",
               netcdf_config->file_path, netcdf_config->variable_name,
               netcdf_config->time_name, netcdf_config->latitude_name,
               netcdf_config->longitude_name) != 5)
      ERR("Invalid weather file line in config file: %s", trimmed);
  }

  // Validate
  for (int i = 0; i < WEATHER_NTYPES; i++) {
    if (strlen(config->weather_files[i].file_path) == 0) {
      ERR("Weather file for type %s is not specified in the configuration.\n",
          WEATHER_VARIABLES[i]);
    }
  }
}

static void ReadCropConfiguration(FILE *fp, CropConfig *config) {
  char line[MAX_STRING], option[MAX_STRING], type[MAX_STRING];
  int used, used2;

  // Clear
  config->plant_date = -1;
  config->emergence = -1;
  memset(config->crop_name, 0, MAX_STRING);
  memset(config->crop_file, 0, MAX_STRING);
  memset(config->management_file, 0, MAX_STRING);
  memset(config->soil_file, 0, MAX_STRING);
  memset(config->site_file, 0, MAX_STRING);
  for (int i = 0; i < DOMAIN_NTYPES; i++) {
    memset(&config->domain_files[i], 0, sizeof(NetCDFConfig));
  }

  bool found = false;
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line, strnlen(line, MAX_STRING));
    if (skip_comment(trimmed))
      continue;

    if (sscanf(trimmed, "%s %n", option, &used) != 1)
      ERR("Invalid line in config file: %s", trimmed);

    if (!found && strcmp(option, CROP_NAME_OPTION) != 0)
      continue;

    if (strcmp(option, CROP_NAME_OPTION) == 0) {
      if (found) {
        fseek(fp, -strlen(line), SEEK_CUR);
        break; // Stop reading if we already found a crop name
      }
      found = true;

      if (strlen(config->crop_name) != 0)
        ERR("Duplicate %s line in config file: %s", CROP_NAME_OPTION, trimmed);
      if (sscanf(trimmed + used, "%s", config->crop_name) != 1)
        ERR("Invalid %s line in config file: %s", CROP_NAME_OPTION, trimmed);
    } else if (strcmp(option, PLANT_DATE_OPTION) == 0) {
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
    } else {
      ERR("Unknown option in crop configuration: %s", option);
    }
  }

  if (!found)
    ERR("No crop configuration found in config file.");

  // Validate
  if (strlen(config->crop_name) == 0)
    ERR("Crop name must be specified in the crop configuration.");
  if (config->plant_date == -1)
    ERR("Plant date must be specified in the crop configuration.");
  if (config->emergence == -1)
    ERR("Emergence must be specified in the crop configuration.");
  if (strlen(config->crop_file) == 0)
    ERR("Crop file must be specified in the crop configuration.");
  if (strlen(config->management_file) == 0)
    ERR("Management file must be specified in the crop configuration.");
  if (strlen(config->soil_file) == 0)
    ERR("Soil file must be specified in the crop configuration.");
  if (strlen(config->site_file) == 0)
    ERR("Site file must be specified in the crop configuration.");
  for (int i = 0; i < DOMAIN_NTYPES; i++) {
    if (strlen(config->domain_files[i].file_path) == 0) {
      ERR("Domain file for type %s is not specified in the crop "
          "configuration.\n",
          DOMAIN_VARIABLES[i]);
    }
  }
}

static void ReadCropsConfiguration(FILE *fp, Config *config) {
  DBG("ReadCropsConfiguration");

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

  // Read
  rewind(fp);
  for (size_t i = 0; i < config->CropSize; i++) {
    CropConfig *crop_config = &config->crop_configurations[i];
    ReadCropConfiguration(fp, crop_config);
  }

  // Validate
  for (size_t i = 0; i < config->CropSize; i++) {
    CropConfig *crop_config_i = &config->crop_configurations[i];
    for (size_t j = i + 1; j < config->CropSize; j++) {
      CropConfig *crop_config_j = &config->crop_configurations[j];
      if (strcmp(crop_config_i->crop_name, crop_config_j->crop_name) == 0) {
        ERR("Duplicate crop name found in crop configurations: %s",
            crop_config_i->crop_name);
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
  ReadWeatherConfiguration(fp, configuration);
  rewind(fp);
  ReadCropsConfiguration(fp, configuration);

  fclose(fp);
}
