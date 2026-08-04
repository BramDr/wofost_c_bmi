#include <stdio.h>
#include <stdlib.h>

#include "bmi_wofost.h"
#include "defs.h"
#include <bmi.h>

int main(const int argc, const char *argv[]) {
  if (argc != 2)
    ERR("Usage: %s CONFIGURATION_FILERun the wofost model through its "
        "BMI with a configuration file.",
        argv[0]);

  const char *config_file = argv[1];

  Bmi *model = (Bmi *)malloc(sizeof(Bmi));
  if (model == NULL)
    ERR("Failed to allocate memory for the BMI model.");

  INFO("Registering model");
  if (RegisterBmiWofost(model) == NULL)
    ERR("Failed to register BMI model.");

  char component_name[BMI_MAX_COMPONENT_NAME];
  if (model->get_component_name(model, component_name) != BMI_SUCCESS)
    ERR("Failed to get component name");

  INFO("Initializing %s with configuration file %s", component_name,
       config_file);
  if (model->initialize(model, config_file) != BMI_SUCCESS)
    ERR("Failed to initialize component");

  double start_time, end_time, time_step;
  if (model->get_start_time(model, &start_time) != BMI_SUCCESS)
    ERR("Failed to get start time");
  if (model->get_end_time(model, &end_time) != BMI_SUCCESS)
    ERR("Failed to get end time");
  if (model->get_time_step(model, &time_step) != BMI_SUCCESS)
    ERR("Failed to get time step");

  INFO("Running %s from time %f to %f with time step %f", component_name,
       start_time, end_time, time_step);
  for (double time = start_time; time < end_time; time += time_step) {

    time_t time_time_t = REFERENCE_TIME_T + (time_t)time;
    struct tm *time_tm = gmtime(&time_time_t);
    INFO("Updating %s at time %04d-%02d-%02d", component_name,
         time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday);

    if (model->update(model) != BMI_SUCCESS)
      ERR("Failed to update component");
  }

  INFO("Finalizing %s", component_name);
  if (model->finalize(model) != BMI_SUCCESS)
    ERR("Failed to finalize component");

  free(model);
  return EXIT_SUCCESS;
}