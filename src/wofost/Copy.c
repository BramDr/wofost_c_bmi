#include "defs.h"
#include "wofost.h"

static TABLE *copy_table(const TABLE *src) {
  TABLE *head = NULL;
  TABLE **tail = &head;
  for (const TABLE *n = src; n != NULL; n = n->next) {
    TABLE *node = malloc(sizeof(TABLE));
    if (node == NULL)
      ERR(printf("could not allocate TABLE node."));
    *node = *n;
    node->next = NULL;
    *tail = node;
    tail = &node->next;
  }
  return head;
}

static TABLE_D *copy_table_d(const TABLE_D *src) {
  TABLE_D *head = NULL;
  TABLE_D **tail = &head;
  for (const TABLE_D *n = src; n != NULL; n = n->next) {
    TABLE_D *node = malloc(sizeof(TABLE_D));
    if (node == NULL)
      ERR(printf("could not allocate TABLE_D node."));
    *node = *n;
    node->next = NULL;
    *tail = node;
    tail = &node->next;
  }
  return head;
}

static Green *copy_green(const Green *src) {
  Green *head = NULL;
  Green **tail = &head;
  for (const Green *n = src; n != NULL; n = n->next) {
    Green *node = malloc(sizeof(Green));
    if (node == NULL)
      ERR(printf("could not allocate Green node."));
    *node = *n;
    node->next = NULL;
    *tail = node;
    tail = &node->next;
  }
  return head;
}

static void copy_parameters(const Parameters *from, Parameters *to) {
  *to = *from;
  to->Roots = copy_table(from->Roots);
  to->Stems = copy_table(from->Stems);
  to->Leaves = copy_table(from->Leaves);
  to->Storage = copy_table(from->Storage);
  to->VernalizationRate = copy_table(from->VernalizationRate);
  to->DeltaTempSum = copy_table(from->DeltaTempSum);
  to->SpecificLeaveArea = copy_table(from->SpecificLeaveArea);
  to->SpecificStemArea = copy_table(from->SpecificStemArea);
  to->KDiffuseTb = copy_table(from->KDiffuseTb);
  to->EFFTb = copy_table(from->EFFTb);
  to->MaxAssimRate = copy_table(from->MaxAssimRate);
  to->FactorAssimRateTemp = copy_table(from->FactorAssimRateTemp);
  to->FactorGrossAssimTemp = copy_table(from->FactorGrossAssimTemp);
  to->FactorSenescence = copy_table(from->FactorSenescence);
  to->DeathRateStems = copy_table(from->DeathRateStems);
  to->DeathRateRoots = copy_table(from->DeathRateRoots);
  to->CO2AMAXTB = copy_table(from->CO2AMAXTB);
  to->CO2EFFTB = copy_table(from->CO2EFFTB);
  to->CO2TRATB = copy_table(from->CO2TRATB);
  to->N_MaxLeaves = copy_table(from->N_MaxLeaves);
  to->P_MaxLeaves = copy_table(from->P_MaxLeaves);
  to->K_MaxLeaves = copy_table(from->K_MaxLeaves);
}

static void copy_plant(const Plant *from, Plant *to) {
  *to = *from;
  copy_parameters(&from->prm, &to->prm);
  to->LeaveProperties = copy_green(from->LeaveProperties);
}

static void copy_field(const Field *from, Field *to) {
  *to = *from;
  to->NotInfTB = copy_table(from->NotInfTB);
}

static void copy_management(const Management *from, Management *to) {
  *to = *from;
  to->N_Fert_table = copy_table_d(from->N_Fert_table);
  to->P_Fert_table = copy_table_d(from->P_Fert_table);
  to->K_Fert_table = copy_table_d(from->K_Fert_table);
  to->Irrigation = copy_table_d(from->Irrigation);
}

static void copy_soil(const Soil *from, Soil *to) {
  *to = *from;
  to->VolumetricSoilMoisture = copy_table(from->VolumetricSoilMoisture);
  to->HydraulicConductivity = copy_table(from->HydraulicConductivity);
}

void CopySimUnit(const SimUnit *from, SimUnit *to) {
  *to = *from;
  copy_plant(&from->crp, &to->crp);
  copy_field(&from->ste, &to->ste);
  copy_management(&from->mng, &to->mng);
  copy_soil(&from->soil, &to->soil);
}
