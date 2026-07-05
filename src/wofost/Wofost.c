#include "wofost.h"

void UpdateWofost() {
  int Emergence = SUnit->emergence;
  int CycleLength = 300;

  Temp = 0.5 * (Meteo->Tmax + Meteo->Tmin);
  DayTemp = 0.5 * (Meteo->Tmax + Temp);

  shift_down_float(Meteo->Tmin_history, TMIN_HISTORY_LENGTH);
  Meteo->Tmin_history[TMIN_HISTORY_LENGTH - 1] = Meteo->Tmin;

  /* Determine if the sowing already has occurred */
  IfSowing(SUnit->start);

  /* If sowing has occurred than determine the emergence */
  if (Crop->Sowing >= 1 && Crop->Emergence == 0) {
    if (EmergenceCrop(Emergence)) {
      /* Initialize: set state variables */
      InitializeCrop();
      InitializeWatBal();
      InitializeNutrients();
    }
  }

  if (Crop->Sowing >= 1 && Crop->Emergence == 1) {
    if (Crop->st.Development <= (Crop->prm.DevelopStageHarvest) &&
        Crop->GrowthDay < CycleLength) {
      Astro();
      CalcPenman();
      CalcPenmanMonteith();

      /* Calculate the evapotranspiration */
      EvapTra();

      /* Set the rate variables to zero */
      RatesToZero();

      /* Rate calculations */
      if (Standalone)
        RateCalulationWatBal();
      Partioning();
      RateCalcultionNutrients();
      RateCalculationCrop();

      /* Calculate LAI */
      Crop->st.LAI = LeaveAreaIndex();

      /* State calculations */
      IntegrationCrop();
      if (Standalone)
        IntegrationWatBal();
      IntegrationNutrients();

      /* Update the number of days that the crop has grown*/
      Crop->GrowthDay++;
    } else {
      Emergence = 0;
      Crop->TSumEmergence = 0;
      Crop->Emergence = 0;
      Crop->Sowing = 0;
    }
  }
}