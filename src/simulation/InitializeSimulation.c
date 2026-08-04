#include "simulation.h"
#include "time_utils.h"

void InitializeSimulation(const char *config_file) {
  DBG("InitializeSimulation");

  Configuration = malloc(sizeof(Config));

  ReadConfiguration(config_file, Configuration);
  Start = Configuration->Start;
  End = Configuration->End;
  Standalone = Configuration->standalone;
  IgnoreNutrientStress = Configuration->ignore_nutrient_stress;
  IgnoreSoilMoisture = Configuration->ignore_soil_moisture;

  InitializeDomainUnits();
  InitializeSimulationUnits();
  if (Standalone)
    InitializeMeteo();
  InitializeOutput();

  free(Configuration->crop_configurations);
  free(Configuration);
  Configuration = NULL;

  CurrentTime = timegm_portable(&Start);
  CurrentStep = 0;
}
