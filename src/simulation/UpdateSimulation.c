#include "simulation.h"
#include <math.h>

void UpdateSimulation(void) {
  DBG("UpdateSimulation");

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
      if (isnan(Meteo->Tmin) || isnan(Meteo->Tmax) || isnan(Meteo->Radiation) ||
          isnan(Meteo->Rain) || isnan(Meteo->Windspeed) ||
          isnan(Meteo->Vapour)) {
        ERR("Missing weather data at time %ld for crop %zu, unit %zu.",
            CurrentTime, i, j);
      }
      UpdateWofost();
    }
  }

  UpdateOutput();

  CurrentTime += TIME_STEP;
  CurrentStep++;
}