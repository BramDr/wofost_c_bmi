#ifndef WOFOST_H
#define WOFOST_H

#include "crop.h"
#include "defs.h"
#include "location.h"
#include "manage.h"
#include "meteo.h"
#include "site.h"
#include "soil.h"
#include <stdbool.h>
#include <stddef.h>

#define TIME_STEP 86400.0 // seconds

typedef struct DOMUNIT {
  Location loc;
  Weather met;
} DomUnit;

typedef struct SIMUNIT {
  DomUnit *dom;
  Plant crp;
  Field ste;
  Management mng;
  Soil soil;
  int emergence;
  int start;
} SimUnit;

extern float Step; // days
extern time_t CurrentTime;
extern size_t CurrentStep;
extern DomUnit *DUnit;
extern SimUnit *SUnit;
extern bool Standalone;
extern float Temp;
extern float DayTemp;

/* General help functions */
extern void RatesToZero();
extern void IfSowing();

/* Crop growth */
extern void Partioning();
extern void HeatStress();
extern void RateCalculationCrop();
extern void Growth(float NewPlantMaterial);
extern void IntegrationCrop();
extern void InitializeCrop();
extern int EmergenceCrop(int Emergence);

extern void DevelopmentRate();
extern void LeaveGrowth();
extern float DailyTotalAssimilation();
extern float DyingLeaves();
extern float InstantAssimilation(float KDiffuse, float EFF, float AssimMax,
                                 float SinB, float PARDiffuse, float PARDirect);
extern float LeaveAreaIndex();
extern float Correct(float GrossAssimilation);
extern float RespirationRef(float TotalAssimilation);
extern float Conversion(float NetAssimilation);

/* Nutrients */
extern void CropNutrientRates();
extern void InitializeNutrients();
extern void IntegrationNutrients();
extern void NutritionINDX();
extern void NutrientLoss();
extern void NutrientMax();
extern void NutrientPartioning();
extern void NutrientRates();
extern void NutrientOptimum();
extern void NutrientDemand();
extern void SoilNutrientRates();
extern void NutrientTranslocation();
extern void RateCalcultionNutrients();

/* Water balance */
extern void InitializeWatBal();
extern void RateCalulationWatBal();
extern void IntegrationWatBal();
extern void EvapTra();

/* Wofost */
extern void UpdateWofost();

#endif // WOFOST_H