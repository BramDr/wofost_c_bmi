#include "bmi_wofost.h"
#include <string.h>

const BmiVar *FindBmiVariable(const char *name) {
  for (int i = 0; i < N_BMI_INPUT_VARS; i++) {
    if (strcmp(BMI_INPUT_VARS[i].name, name) == 0) {
      return &BMI_INPUT_VARS[i];
    }
  }
  for (int i = 0; i < N_BMI_OUTPUT_VARS; i++) {
    if (strcmp(BMI_OUTPUT_VARS[i].name, name) == 0) {
      return &BMI_OUTPUT_VARS[i];
    }
  }
  return NULL;
}
