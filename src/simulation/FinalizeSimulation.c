#include "simulation.h"

void FinalizeSimulation(void) {
  DBG("FinalizeSimulation");

  FinalizeSimulationUnits();
  FinalizeDomainUnits();
  if (Standalone)
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