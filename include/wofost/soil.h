#ifndef SOIL_H
#define SOIL_H

#include "defs.h"

#define NR_VARIABLES_SOIL 12
#define NR_VARIABLES_SOIL_USED 6
#define NR_TABLES_SOIL 2

typedef struct CONSTANTS {
  float MaxEvapWater;
  float MoistureFC;
  float MoistureWP;
  float MoistureSAT;
  float CriticalSoilAirC;
  float MaxPercolRTZ;
  float MaxPercolSubS;
  float MaxSurfaceStorge;
  float K0;
} Constants;

typedef struct STATES {
  float EvapWater;
  float EvapSoil;
  float Infiltration;
  float Irrigation;
  float Loss;
  float Moisture;
  float MoistureLOW;
  float Percolation;
  float Rain;
  float RootZoneMoisture;
  float Runoff;
  float SurfaceStorage;
  float Transpiration;
  float WaterRootExt;
} States;

typedef struct RATES {
  float EvapWater;
  float EvapSoil;
  float Infiltration;
  float Irrigation;
  float Loss;
  float Moisture;
  float MoistureLOW;
  float Percolation;
  float RootZoneMoisture;
  float Runoff;
  float Transpiration;
  float WaterRootExt;
} Rates;

typedef struct SOIL {
  float DaysSinceLastRain;
  float SoilMaxRootingDepth;
  float WaterStress;
  float InfPreviousDay;

  /* Tables for Soil */
  TABLE *VolumetricSoilMoisture;
  TABLE *HydraulicConductivity; /* currently not used */

  Constants ct;
  States st;
  Rates rt;
} Soil;

extern Soil *WatBal;

extern void GetSoilData(Soil *SOIL, char *soilfile);
extern void FillSoilVariables(Soil *SOIL, float *Variable);

#endif // SOIL_H