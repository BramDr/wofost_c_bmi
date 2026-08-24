#include "wofost.h"
#include "astro.h"
#include "penman.h"

void UpdateWofost(void) {
  int Emergence = SUnit->emergence;
  int CycleLength = 300;

  Temp = 0.5 * (Meteo->Tmax + Meteo->Tmin);
  DayTemp = 0.5 * (Meteo->Tmax + Temp);

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
      RateCalulationWatBal();
      Partioning();
      RateCalcultionNutrients();
      RateCalculationCrop();

      /* Calculate LAI */
      Crop->st.LAI = LeaveAreaIndex();

      /* State calculations */
      IntegrationCrop();
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