#ifndef MANAGE_H
#define MANAGE_H

#include "defs.h"

#define NR_VARIABLES_MANAGEMENT 9
#define NR_TABLES_MANAGEMENT 4

char *ManageParam[] = {"NRFTAB", "PRFTAB",  "KRFTAB", "NMINS",   "RTNMINS",
                       "PMINS",  "RTPMINS", "KMINS",  "RTKMINS", "NULL"};

char *ManageParam2[] = {"FERNTAB", "FERPTAB", "FERKTAB", "IRRTAB", "NULL"};

typedef struct MANAGEMENT {
  /** Tables for fertilizer application and recovery fraction **/
  TABLE_D *N_Fert_table;
  TABLE_D *P_Fert_table;
  TABLE_D *K_Fert_table;
  TABLE_D *Irrigation;

  float N_Mins;
  float NRecoveryFrac;
  float P_Mins;
  float PRecoveryFrac;
  float K_Mins;
  float KRecoveryFrac;
  float N_Uptake_frac;
  float P_Uptake_frac;
  float K_Uptake_frac;
} Management;

extern Management *Mng;

extern void GetManagement();
extern void FillManageVariables();

#endif // MANAGE_H