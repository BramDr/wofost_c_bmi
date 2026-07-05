#include "simulation.h"

void FinalizeSimulation(void) {
  FinalizeSimulationUnits();
  FinalizeDomainUnits();
  FinalizeMeteo();
  FinalizeOutput();
  Meteo = NULL;
  Loc = NULL;
  Crop = NULL;
  Site = NULL;
  WatBal = NULL;
  Mng = NULL;
  DUnit = NULL;
  SUnit = NULL;
}