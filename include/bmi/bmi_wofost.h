#ifndef BMI_WOFOST_H
#define BMI_WOFOST_H

#include "bmi.h"
#include "defs.h"
#include <stddef.h>

#define GRID_ID 0
#define GRID_LOCATION "node"
#define GRID_TYPE "uniform_rectilinear"
#define REFERENCE_TIME_T 0 // time_t representation of 1970-01-01 00:00:00 UTC

typedef struct {
  const char name[MAX_STRING];
  int grid;
  const char type[MAX_STRING];  /* e.g. "float", "double" */
  const char units[MAX_STRING]; /* CF-convention unit string */
  int itemsize;                 /* sizeof one element in bytes */
  const char *location;         /* "node", "face", or "edge" */
} BmiVar;

static const BmiVar BMI_INPUT_VARS[] = {
    /* name              grid     type      units          itemsize location */
    {"rainfall_flux", GRID_ID, FLOAT_TYPE, "kg m-2 s-1", sizeof(float),
     GRID_LOCATION},
};
static const int N_BMI_INPUT_VARS =
    sizeof(BMI_INPUT_VARS) / sizeof(BMI_INPUT_VARS[0]);

static const BmiVar BMI_OUTPUT_VARS[] = {
    /* name         grid     type      units     itemsize        location */
    {"crop_lai", GRID_ID, FLOAT_TYPE, "m2 m-2", sizeof(float), GRID_LOCATION},
};
static const int N_BMI_OUTPUT_VARS =
    sizeof(BMI_OUTPUT_VARS) / sizeof(BMI_OUTPUT_VARS[0]);

extern Bmi *RegisterBmiWofost(Bmi *model);

extern const BmiVar *FindBmiVariable(const char *name);

#endif // BMI_WOFOST_H
