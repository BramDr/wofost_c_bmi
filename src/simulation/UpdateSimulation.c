#include "simulation.h"

void UpdateSimulation() {
  if (Standalone)
    UpdateMeteo();

  for (size_t i = 0; i < CropSize; i++) {
    for (size_t j = 0; j < ActiveSize[i]; j++) {
      SUnit = &SimGrid[i][j];
      DUnit = SUnit->dom;
      Loc = &DUnit->loc;
      Meteo = &DUnit->met;
      Crop = &SUnit->crp;
      Site = &SUnit->ste;
      Mng = &SUnit->mng;
      WatBal = &SUnit->soil;
      UpdateWofost();
    }
  }

  UpdateOutput();

  CurrentTime += TIME_STEP;
  CurrentStep++;
}