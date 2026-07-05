#ifndef CROP_H
#define CROP_H

#include "defs.h"

#define NR_VARIABLES_CRP 66
#define NR_TABLES_CRP 22

char *CropParam[] = {"TBASEM",
                     "TEFFMX",
                     "TSUMEM",
                     "IDSL",
                     "DLO",
                     "DLC",
                     "VERNSAT",
                     "VERNBASE",
                     "TSUM1",
                     "TSUM2",
                     "DVSI",
                     "DVSEND",
                     "TDWI",
                     "RGRLAI",
                     "SPA",
                     "SPAN",
                     "TBASE",
                     "CVL",
                     "CVO",
                     "CVR",
                     "CVS",
                     "Q10",
                     "RML",
                     "RMO",
                     "RMR",
                     "RMS",
                     "PERDL",
                     "CFET",
                     "DEPNR",
                     "IAIRDU",
                     "RDI",
                     "RRI",
                     "RDMCR",
                     "RDRLV_NPK",
                     "DVS_NPK_STOP",
                     "DVS_NPK_TRANSL",
                     "NPK_TRANSLRT_FR",
                     "NCRIT_FR",
                     "PCRIT_FR",
                     "KCRIT_FR",
                     "NMAXRT_FR",
                     "NMAXST_FR",
                     "PMAXRT_FR",
                     "PMAXST_FR",
                     "KMAXRT_FR",
                     "KMAXST_FR",
                     "NLAI_NPK",
                     "NLUE_NPK",
                     "NMAXSO",
                     "PMAXSO",
                     "KMAXSO",
                     "NPART",
                     "NSLA_NPK",
                     "NRESIDLV",
                     "NRESIDST",
                     "NRESIDRT",
                     "PRESIDLV",
                     "PRESIDST",
                     "PRESIDRT",
                     "KRESIDLV",
                     "KRESIDST",
                     "KRESIDRT",
                     "TCNT",
                     "TCPT",
                     "TCKT",
                     "NFIX_FR",
                     "NULL"};

char *CropParam2[] = {"VERNRTB",   "DTSMTB",    "SLATB",  "SSATB",  "KDIFTB",
                      "EFFTB",     "AMAXTB",    "TMPFTB", "TMNFTB", "CO2AMAXTB",
                      "CO2EFFTB",  "CO2TRATB",  "RFSETB", "FRTB",   "FLTB",
                      "FSTB",      "FOTB",      "RDRRTB", "RDRSTB", "NMAXLV_TB",
                      "PMAXLV_TB", "KMAXLV_TB", "NULL"};

typedef struct PARAMETERS {
  /** Tables for the Crop simulations **/
  TABLE *Roots;
  TABLE *Stems;
  TABLE *Leaves;
  TABLE *Storage;

  TABLE *VernalizationRate;
  TABLE *DeltaTempSum;
  TABLE *SpecificLeaveArea;
  TABLE *SpecificStemArea;
  TABLE *KDiffuseTb;
  TABLE *EFFTb;
  TABLE *MaxAssimRate;
  TABLE *FactorAssimRateTemp;
  TABLE *FactorGrossAssimTemp;
  TABLE *FactorSenescence;
  TABLE *DeathRateStems;
  TABLE *DeathRateRoots;

  /** Tables to account for the atmospheric CO2 concentration **/
  TABLE *CO2AMAXTB;
  TABLE *CO2EFFTB;
  TABLE *CO2TRATB;

  /** Tables for the maximum nutrient content in leaves as a function of DVS **/
  TABLE *N_MaxLeaves;
  TABLE *P_MaxLeaves;
  TABLE *K_MaxLeaves;

  /** Static Variables  **/
  /**  Emergence  **/
  float TempBaseEmergence;
  float TempEffMax;
  float TSumEmergence;

  /**  Phenology  **/
  int IdentifyAnthesis;
  float OptimumDaylength;
  float CriticalDaylength;
  float SatVernRequirement;
  float BaseVernRequirement;
  float TempSum1;
  float TempSum2;
  float InitialDVS;
  float DevelopStageHarvest;

  /** Initial Values  **/
  float InitialDryWeight;
  float RelIncreaseLAI;

  /**  Green Area  **/
  float SpecificPodArea;
  float LifeSpan;
  float TempBaseLeaves;

  /** Conversion assimilates into biomass **/
  float ConversionLeaves;
  float ConversionStorage;
  float ConversionRoots;
  float ConversionStems;

  /** Maintenance Respiration **/
  float Q10;
  float RelRespiLeaves;
  float RelRespiStorage;
  float RelRespiRoots;
  float RelRespiStems;

  /** Death Rates  **/
  float MaxRelDeathRate;

  /** Water Use  **/
  float CorrectionTransp;
  float CropGroupNumber;
  float Airducts;

  /** Rooting **/
  float InitRootingDepth;
  float MaxIncreaseRoot;
  float MaxRootingDepth;

  /** Nutrients **/
  float DyingLeaves_NPK_Stress;
  float DevelopmentStageNLimit;
  float DevelopmentStageNT;
  float FracTranslocRoots;
  float Opt_N_Frac;
  float Opt_P_Frac;
  float Opt_K_Frac;
  float N_MaxRoots;
  float N_MaxStems;
  float P_MaxRoots;
  float P_MaxStems;
  float K_MaxRoots;
  float K_MaxStems;
  float NitrogenStressLAI;
  float NLUE;
  float Max_N_storage;
  float Max_P_storage;
  float Max_K_storage;
  float N_lv_partitioning;
  float NutrientStessSLA;
  float N_ResidualFrac_lv;
  float N_ResidualFrac_st;
  float N_ResidualFrac_ro;
  float P_ResidualFrac_lv;
  float P_ResidualFrac_st;
  float P_ResidualFrac_ro;
  float K_ResidualFrac_lv;
  float K_ResidualFrac_st;
  float K_ResidualFrac_ro;
  float TCNT;
  float TCPT;
  float TCKT;
  float N_fixation;
} Parameters;

typedef struct NUTRIENT_RATES {
  float roots;
  float stems;
  float leaves;
  float storage;
  float Demand_lv;
  float Demand_st;
  float Demand_ro;
  float Demand_so;
  float Supply;
  float Transloc;
  float Transloc_lv;
  float Transloc_st;
  float Transloc_ro;
  float Uptake;
  float Uptake_lv;
  float Uptake_st;
  float Uptake_ro;
  float death_lv;
  float death_st;
  float death_ro;
} nutrient_rates;

typedef struct NUTRIENT_STATES {
  float roots;
  float stems;
  float leaves;
  float storage;
  float Max_lv;
  float Max_st;
  float Max_ro;
  float Max_so;
  float Optimum_lv;
  float Optimum_st;
  float Indx;
  float Uptake;
  float Uptake_lv;
  float Uptake_st;
  float Uptake_ro;
  float death_lv;
  float death_st;
  float death_ro;
  float Avail;
  float Avail_lv;
  float Avail_st;
  float Avail_ro;

} nutrient_states;

typedef struct GROWTH_RATES {
  float roots;
  float stems;
  float leaves;
  float LAIExp;
  float storage;
  float Development;
  float RootDepth;
  float vernalization;
} growth_rates;

typedef struct GROWTH_STATES {
  float roots;
  float stems;
  float leaves;
  float LAI;
  float LAIExp;
  float storage;
  float Development;
  float RootDepth;
  float RootDepth_prev;
  float vernalization;
} growth_states;

typedef struct DYING_STATES {
  float roots;
  float stems;
  float leaves;
} dying_states;

typedef struct DYING_RATES {
  float roots;
  float stems;
  float leaves;
} dying_rates;

typedef struct GREEN {
  float weight;
  float age;
  float area;
  struct GREEN *next;
} Green;

typedef struct PLANT {
  int Emergence;
  int Sowing;
  int GrowthDay;
  float NPK_Indx;
  float NutrientStress;
  float DaysOxygenStress;
  float TSumEmergence;
  float fac_ro;
  float fac_lv;
  float fac_st;
  float fac_so;

  Parameters prm;

  growth_rates rt;
  growth_states st;
  dying_rates drt;
  dying_states dst;

  nutrient_states N_st;
  nutrient_states P_st;
  nutrient_states K_st;

  nutrient_rates N_rt;
  nutrient_rates P_rt;
  nutrient_rates K_rt;

  Green *LeaveProperties;
} Plant;

extern Plant *Crop;

extern void GetCropData();
extern void FillCropVariables();

#endif // CROP_H
