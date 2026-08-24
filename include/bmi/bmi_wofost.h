#ifndef BMI_WOFOST_H
#define BMI_WOFOST_H

#include <bmi.h>
#include <stddef.h>

#define GRID_ID 0
#define GRID_LOCATION "node"
#define GRID_TYPE "uniform_rectilinear"
#define REFERENCE_TIME_T 0 // time_t representation of 1970-01-01 00:00:00 UTC

enum {
  // Static
  BMI_INPUT_SOIL_AERATED,
  BMI_INPUT_SOIL_FIELD_CAPACITY,
  BMI_INPUT_SOIL_WILTING_POINT,
  BMI_INPUT_SOIL_SATURATED,
  // Dynamic
  BMI_INPUT_PRECIPITATION,
  BMI_INPUT_MAXIMUM_TEMPERATURE,
  BMI_INPUT_MINIMUM_TEMPERATURE,
  BMI_INPUT_WIND_SPEED,
  BMI_INPUT_SHORTWAVE_RADIATION,
  BMI_INPUT_VAPOR_PRESSURE,
  // Dynamic per crop
  BMI_INPUT_SOIL_SATURATION,
  BMI_NINPUTS
};

enum {
  BMI_OUTPUT_LEAF_AREA_INDEX,
  BMI_OUTPUT_ROOT_DEPTH,
  BMI_OUTPUT_EVAPOTRANSPIRATION,
  BMI_NOUTPUTS
};

typedef struct {
  const int index;
  const char name[BMI_MAX_VAR_NAME];
  const int grid;
  const char type[BMI_MAX_TYPE_NAME];   /* e.g. "float", "double" */
  const char units[BMI_MAX_UNITS_NAME]; /* CF-convention unit string */
  const int itemsize;                   /* sizeof one element in bytes */
  const char *location;                 /* "node", "face", or "edge" */
} BmiVar;

extern Bmi *register_bmi_wofost(Bmi *model);

#endif // BMI_WOFOST_H
