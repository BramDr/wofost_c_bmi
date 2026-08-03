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
extern bool IgnoreNutrientStress;
extern bool IgnoreSoilMoisture;
extern float Temp;
extern float DayTemp;

/* General help functions */
extern void RatesToZero(void);
extern void IfSowing(const int start);
extern void Clean(void);
extern void CopySimUnit(const SimUnit *from, SimUnit *to);

/* Crop growth */
extern void Partioning(void);
extern void HeatStress(void);
extern void RateCalculationCrop(void);
extern void Growth(float NewPlantMaterial);
extern void IntegrationCrop(void);
extern void InitializeCrop(void);
extern int EmergenceCrop(int Emergence);

extern void DevelopmentRate(void);
extern void LeaveGrowth(void);
extern float DailyTotalAssimilation(void);
extern float DyingLeaves(void);
extern float InstantAssimilation(float KDiffuse, float EFF, float AssimMax,
                                 float SinB, float PARDiffuse, float PARDirect);
extern float LeaveAreaIndex(void);
extern float Correct(float GrossAssimilation);
extern float RespirationRef(float TotalAssimilation);
extern float Conversion(float NetAssimilation);

/* Nutrients */
extern void CropNutrientRates(void);
extern void InitializeNutrients(void);
extern void IntegrationNutrients(void);
extern void NutritionINDX(void);
extern void NutrientLoss(void);
extern void NutrientMax(void);
extern void NutrientPartioning(void);
extern void NutrientRates(void);
extern void NutrientOptimum(void);
extern void NutrientDemand(void);
extern void SoilNutrientRates(void);
extern void NutrientTranslocation(void);
extern void RateCalcultionNutrients(void);

/* Water balance */
extern void InitializeWatBal(void);
extern void RateCalulationWatBal(void);
extern void IntegrationWatBal(void);
extern void EvapTra(void);

/* Wofost */
extern void UpdateWofost(void);

#endif // WOFOST_H