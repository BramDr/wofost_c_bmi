#include <stdio.h>
#include <stdlib.h>

#include "bmi_wofost.h"
#include "defs.h"
#include <bmi.h>

int main(const int argc, const char *argv[]) {
  if (argc != 2)
    ERR("Usage: %s CONFIGURATION_FILE\nRun the wofost model through its "
        "BMI with a configuration file.",
        argv[0]);

  const char *config_file = argv[1];

  Bmi *model = (Bmi *)malloc(sizeof(Bmi));
  if (model == NULL)
    ERR("Failed to allocate memory for the BMI model.");

  fprintf(stdout, "Registering model\n");
  if (RegisterBmiWofost(model) == NULL)
    ERR("Failed to register BMI model.");

  char component_name[BMI_MAX_COMPONENT_NAME];
  if (model->get_component_name(model, component_name) != BMI_SUCCESS)
    ERR("Failed to get component name");

  fprintf(stdout, "Initializing %s with configuration file %s\n",
          component_name, config_file);
  if (model->initialize(model, config_file) != BMI_SUCCESS)
    ERR("Failed to initialize component");

  double start_time, end_time, time_step;
  if (model->get_start_time(model, &start_time) != BMI_SUCCESS)
    ERR("Failed to get start time");
  if (model->get_end_time(model, &end_time) != BMI_SUCCESS)
    ERR("Failed to get end time");
  if (model->get_time_step(model, &time_step) != BMI_SUCCESS)
    ERR("Failed to get time step");

  fprintf(stdout, "Running %s from time %f to %f with time step %f\n",
          component_name, start_time, end_time, time_step);
  for (double time = start_time; time < end_time; time += time_step) {

    fprintf(stdout, "Updating %s at time %f\n", component_name, time);
    if (model->update(model) != BMI_SUCCESS)
      ERR("Failed to update component");
  }

  fprintf(stdout, "Finalizing %s\n", component_name);
  if (model->finalize(model) != BMI_SUCCESS)
    ERR("Failed to finalize component");

  free(model);
  return EXIT_SUCCESS;
}