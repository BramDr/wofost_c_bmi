#include "wofost.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *CropParam[] = {"TBASEM",
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

static const char *CropParam2[] = {
    "VERNRTB", "DTSMTB",    "SLATB",     "SSATB",     "KDIFTB",   "EFFTB",
    "AMAXTB",  "TMPFTB",    "TMNFTB",    "CO2AMAXTB", "CO2EFFTB", "CO2TRATB",
    "RFSETB",  "FRTB",      "FLTB",      "FSTB",      "FOTB",     "RDRRTB",
    "RDRSTB",  "NMAXLV_TB", "PMAXLV_TB", "KMAXLV_TB", "NULL"};

/* ------------------------------------------------------------------------*/
/*  function GetCropData()                                                 */
/*  Purpose: Read the Wofost crop file and store the parameters and tables */
/* ------------------------------------------------------------------------*/

void GetCropData(Plant *CROP, char *cropfile) {
  TABLE *Table[NR_TABLES_CRP] = {NULL}, *start;

  char line[MAX_STRING];
  int i, c, count;
  float Variable[NR_VARIABLES_CRP], XValue, YValue;
  char x[2], xx[2], word[100];
  FILE *fq;

  if ((fq = fopen(cropfile, "rt")) == NULL) {
    fprintf(stderr, "Cannot open input file %s.\n", cropfile);
    exit(0);
  }

  i = 0;
  count = 0;
  while (strcmp(CropParam[i], "NULL")) {
    while ((c = fscanf(fq, "%s", word)) != EOF) {
      if (strlen(word) > 98) {
        fprintf(stderr, "Check the site input file: very long strings.\n");
        exit(0);
      }
      if (!strcmp(word, CropParam[i])) {
        while ((c = fgetc(fq)) != '=')
          ;
        if (fscanf(fq, "%f", &Variable[i]) != 1) {
          fprintf(stderr, "Cannot read value of %s in file %s.\n",
                  CropParam[i], cropfile);
          exit(0);
        }
        count++;
        break;
      }
    }
    rewind(fq);
    i++;
  }

  if (count == NR_VARIABLES_CRP || count == NR_VARIABLES_CRP - 2)
    ;
  else {
    fprintf(stderr, "Something wrong with the Crop variables in file %s.\n",
            cropfile);
    exit(0);
  }

  rewind(fq);

  FillCropVariables(CROP, Variable);

  i = 0;
  count = 0;
  while (strcmp(CropParam2[i], "NULL")) {
    while (fgets(line, MAX_STRING, fq)) {
      if (line[0] == '*' || line[0] == ' ' || line[0] == '\n' ||
          line[0] == '\r') {
        continue;
      }

      sscanf(line, "%s", word);
      if (!strcmp(word, CropParam2[i])) {

        c = sscanf(line, "%s %s %f %s  %f", word, x, &XValue, xx, &YValue);

        Table[i] = start = malloc(sizeof(TABLE));
        Table[i]->next = NULL;
        Table[i]->x = XValue;
        Table[i]->y = YValue;

        while (fgets(line, MAX_STRING, fq)) {
          if ((c = sscanf(line, " %f %s  %f", &XValue, xx, &YValue)) != 3)
            break;

          Table[i]->next = malloc(sizeof(TABLE));
          Table[i] = Table[i]->next;
          Table[i]->next = NULL;
          Table[i]->x = XValue;
          Table[i]->y = YValue;
        }
        /* Go back to beginning of the table */
        Table[i] = start;
        count++;
        break;
      }
    }
    rewind(fq);
    i++;
  }

  fclose(fq);

  if (count == NR_TABLES_CRP || count == NR_TABLES_CRP - 1)
    ;
  else {
    fprintf(stderr, "Something wrong with the Crop tables in file %s.\n",
            cropfile);
    exit(0);
  }

  if (CROP->prm.IdentifyAnthesis < 2) {
    CROP->prm.VernalizationRate = NULL;
  } else {
    if (Table[0] == NULL)
      ERR("VERNRTB table must be specified in file %s when "
          "vernalization is used (IDSL >= 2).",
          cropfile);
    CROP->prm.VernalizationRate = Table[0];
  }

  for (i = 1; i < NR_TABLES_CRP; i++) {
    if (Table[i] == NULL)
      ERR("Missing required crop table (index %d) in file %s.", i, cropfile);
  }

  CROP->prm.DeltaTempSum = Table[1];
  CROP->prm.SpecificLeaveArea = Table[2];
  CROP->prm.SpecificStemArea = Table[3];
  CROP->prm.KDiffuseTb = Table[4];
  CROP->prm.EFFTb = Table[5];
  CROP->prm.MaxAssimRate = Table[6];
  CROP->prm.FactorAssimRateTemp = Table[7];
  CROP->prm.FactorGrossAssimTemp = Table[8];
  CROP->prm.CO2AMAXTB = Table[9];
  CROP->prm.CO2EFFTB = Table[10];
  CROP->prm.CO2TRATB = Table[11];
  CROP->prm.FactorSenescence = Table[12];
  CROP->prm.Roots = Table[13];
  CROP->prm.Leaves = Table[14];
  CROP->prm.Stems = Table[15];
  CROP->prm.Storage = Table[16];
  CROP->prm.DeathRateStems = Table[17];
  CROP->prm.DeathRateRoots = Table[18];
  CROP->prm.N_MaxLeaves = Table[19];
  CROP->prm.P_MaxLeaves = Table[20];
  CROP->prm.K_MaxLeaves = Table[21];

  CROP->Emergence = 0;
  CROP->TSumEmergence = 0.;

  /* Crop development has not started yet*/
  CROP->st.RootDepth = 0.;
  CROP->st.RootDepth_prev = 0.;
  CROP->st.Development = 0.;
  CROP->DaysOxygenStress = 0; // No crop development therefore no oxygen stress

  /* No initial nutrient stress */
  CROP->NutrientStress = 1.;
  CROP->NPK_Indx = 1;

  /* STATES */
  /* Set the initial growth states to zero */
  CROP->st.roots = 0.;
  CROP->st.stems = 0.;
  CROP->st.leaves = 0.;
  CROP->st.storage = 0.;
  CROP->st.LAIExp = 0.;
  CROP->st.vernalization = 0.;

  /*Set the initial dying state to zero */
  CROP->dst.leaves = 0.;
  CROP->dst.stems = 0.;
  CROP->dst.roots = 0.;

  /* Set the initial nutrient states to zero*/
  CROP->N_st.leaves = 0.;
  CROP->N_st.stems = 0.;
  CROP->N_st.roots = 0.;
  CROP->N_st.storage = 0.;

  CROP->P_st.leaves = 0.;
  CROP->P_st.stems = 0.;
  CROP->P_st.roots = 0.;
  CROP->P_st.storage = 0.;

  CROP->K_st.leaves = 0.;
  CROP->K_st.stems = 0.;
  CROP->K_st.roots = 0.;
  CROP->K_st.storage = 0.;

  /* Set the maximum nutrient concentration to zero at initialization */
  CROP->N_st.Max_lv = 0.;
  CROP->N_st.Max_st = 0.;
  CROP->N_st.Max_ro = 0.;

  CROP->P_st.Max_lv = 0.;
  CROP->P_st.Max_st = 0.;
  CROP->P_st.Max_ro = 0.;

  CROP->K_st.Max_lv = 0.;
  CROP->K_st.Max_st = 0.;
  CROP->K_st.Max_ro = 0.;

  /* Set the initial optimal leave concentrations to zero */
  CROP->N_st.Optimum_lv = 0;
  CROP->N_st.Optimum_st = 0;

  CROP->P_st.Optimum_lv = 0;
  CROP->P_st.Optimum_st = 0;

  CROP->K_st.Optimum_lv = 0;
  CROP->K_st.Optimum_st = 0;

  /* No nutrient stress at initialization */
  CROP->NPK_Indx = 1.;
  CROP->N_st.Indx = 1.;
  CROP->P_st.Indx = 1.;
  CROP->K_st.Indx = 1.;

  /* Set the initial uptake states to zero*/
  CROP->N_st.Uptake = 0.;
  CROP->N_st.Uptake_lv = 0.;
  CROP->N_st.Uptake_st = 0.;
  CROP->N_st.Uptake_ro = 0.;

  CROP->P_st.Uptake = 0.;
  CROP->P_st.Uptake_lv = 0.;
  CROP->P_st.Uptake_st = 0.;
  CROP->P_st.Uptake_ro = 0.;

  CROP->K_st.Uptake = 0.;
  CROP->K_st.Uptake_lv = 0.;
  CROP->K_st.Uptake_st = 0.;
  CROP->K_st.Uptake_ro = 0.;

  /* No nutrient losses at initialization */
  CROP->N_st.death_lv = 0.;
  CROP->N_st.death_st = 0.;
  CROP->N_st.death_ro = 0.;

  CROP->P_st.death_lv = 0.;
  CROP->P_st.death_st = 0.;
  CROP->P_st.death_ro = 0.;

  CROP->K_st.death_lv = 0.;
  CROP->K_st.death_st = 0.;
  CROP->K_st.death_ro = 0.;
}
