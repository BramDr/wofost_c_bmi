#ifndef SITE_H
#define SITE_H

#include "defs.h"

#define NR_VARIABLES_SITE 12
#define NR_TABLES_SITE 1

char *SiteParam[] = {"IZT",    "IFUNRN", "IDRAIN", "SSMAX", "WAV", "ZTI", "DD",
                     "RDMSOL", "NOTINF", "SSI",    "SMLIM", "CO2", "NULL"};

char *SiteParam2[] = {"NINFTB", "NULL"};

typedef struct FIELD {
  /* Water related parameters */
  float FlagGroundWater;
  float InfRainDependent;
  float FlagDrains;
  float MaxSurfaceStorage;
  float InitSoilMoisture;
  float GroundwaterDepth;
  float DD;
  float SoilLimRootDepth;
  float NotInfiltrating;
  float SurfaceStorage;
  float MaxInitSoilM;
  float CO2; /* atmospheric CO2 concentration in ppm */

  /* Mineral states and rates */
  float st_N_tot;
  float st_P_tot;
  float st_K_tot;

  float st_N_mins;
  float st_P_mins;
  float st_K_mins;

  float rt_N_tot;
  float rt_P_tot;
  float rt_K_tot;

  float rt_N_mins;
  float rt_P_mins;
  float rt_K_mins;

  /** Table for the fraction of precipitation that does not infiltrate **/
  TABLE *NotInfTB;
} Field;

extern Field *Site;

extern void GetSiteData();
extern void FillSiteVariables();

#endif // SITE_H
