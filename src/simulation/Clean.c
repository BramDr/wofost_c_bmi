#include <stdio.h>
#include <stdlib.h>
#include "wofost.h"

/* ---------------------------------------------------------------*/
/*  function Clean()                                              */
/*  Purpose: free all the allocated memory and set nodes to NULL  */
/* ---------------------------------------------------------------*/

void Clean()
{
    Green *LeaveProperties;
    TABLE *head;
    TABLE_D *head_D;

    /* Free all the Afgen tables */
    while (SUnit->crp.prm.VernalizationRate)
    {
        head = SUnit->crp.prm.VernalizationRate;
        SUnit->crp.prm.VernalizationRate = SUnit->crp.prm.VernalizationRate->next;
        free(head);
    }
    free(SUnit->crp.prm.VernalizationRate);
    SUnit->crp.prm.VernalizationRate = NULL;

    while (SUnit->crp.prm.DeltaTempSum)
    {
        head = SUnit->crp.prm.DeltaTempSum;
        SUnit->crp.prm.DeltaTempSum = SUnit->crp.prm.DeltaTempSum->next;
        free(head);
    }
    free(SUnit->crp.prm.DeltaTempSum);
    SUnit->crp.prm.DeltaTempSum = NULL;

    while (SUnit->crp.prm.SpecificLeaveArea)
    {
        head = SUnit->crp.prm.SpecificLeaveArea;
        SUnit->crp.prm.SpecificLeaveArea = SUnit->crp.prm.SpecificLeaveArea->next;
        free(head);
    }
    free(SUnit->crp.prm.SpecificLeaveArea);
    SUnit->crp.prm.SpecificLeaveArea = NULL;

    while (SUnit->crp.prm.SpecificStemArea)
    {
        head = SUnit->crp.prm.SpecificStemArea;
        SUnit->crp.prm.SpecificStemArea = SUnit->crp.prm.SpecificStemArea->next;
        free(head);
    }
    free(SUnit->crp.prm.SpecificStemArea);
    SUnit->crp.prm.SpecificStemArea = NULL;

    while (SUnit->crp.prm.KDiffuseTb)
    {
        head = SUnit->crp.prm.KDiffuseTb;
        SUnit->crp.prm.KDiffuseTb = SUnit->crp.prm.KDiffuseTb->next;
        free(head);
    }
    free(SUnit->crp.prm.KDiffuseTb);
    SUnit->crp.prm.KDiffuseTb = NULL;

    while (SUnit->crp.prm.EFFTb)
    {
        head = SUnit->crp.prm.EFFTb;
        SUnit->crp.prm.EFFTb = SUnit->crp.prm.EFFTb->next;
        free(head);
    }
    free(SUnit->crp.prm.EFFTb);
    SUnit->crp.prm.EFFTb = NULL;

    while (SUnit->crp.prm.MaxAssimRate)
    {
        head = SUnit->crp.prm.MaxAssimRate;
        SUnit->crp.prm.MaxAssimRate = SUnit->crp.prm.MaxAssimRate->next;
        free(head);
    }
    free(SUnit->crp.prm.MaxAssimRate);
    SUnit->crp.prm.MaxAssimRate = NULL;

    while (SUnit->crp.prm.FactorAssimRateTemp)
    {
        head = SUnit->crp.prm.FactorAssimRateTemp;
        SUnit->crp.prm.FactorAssimRateTemp = SUnit->crp.prm.FactorAssimRateTemp->next;
        free(head);
    }
    free(SUnit->crp.prm.FactorAssimRateTemp);
    SUnit->crp.prm.FactorAssimRateTemp = NULL;

    while (SUnit->crp.prm.FactorGrossAssimTemp)
    {
        head = SUnit->crp.prm.FactorGrossAssimTemp;
        SUnit->crp.prm.FactorGrossAssimTemp = SUnit->crp.prm.FactorGrossAssimTemp->next;
        free(head);
    }
    free(SUnit->crp.prm.FactorGrossAssimTemp);
    SUnit->crp.prm.FactorGrossAssimTemp = NULL;

    while (SUnit->crp.prm.CO2AMAXTB)
    {
        head = SUnit->crp.prm.CO2AMAXTB;
        SUnit->crp.prm.CO2AMAXTB = SUnit->crp.prm.CO2AMAXTB->next;
        free(head);
    }
    free(SUnit->crp.prm.CO2AMAXTB);
    SUnit->crp.prm.CO2AMAXTB = NULL;

    while (SUnit->crp.prm.CO2EFFTB)
    {
        head = SUnit->crp.prm.CO2EFFTB;
        SUnit->crp.prm.CO2EFFTB = SUnit->crp.prm.CO2EFFTB->next;
        free(head);
    }
    free(SUnit->crp.prm.CO2EFFTB);
    SUnit->crp.prm.CO2EFFTB = NULL;

    while (SUnit->crp.prm.CO2TRATB)
    {
        head = SUnit->crp.prm.CO2TRATB;
        SUnit->crp.prm.CO2TRATB = SUnit->crp.prm.CO2TRATB->next;
        free(head);
    }
    free(SUnit->crp.prm.CO2TRATB);
    SUnit->crp.prm.CO2TRATB = NULL;

    while (SUnit->crp.prm.FactorSenescence)
    {
        head = SUnit->crp.prm.FactorSenescence;
        SUnit->crp.prm.FactorSenescence = SUnit->crp.prm.FactorSenescence->next;
        free(head);
    }
    free(SUnit->crp.prm.FactorSenescence);
    SUnit->crp.prm.FactorSenescence = NULL;

    while (SUnit->crp.prm.Roots)
    {
        head = SUnit->crp.prm.Roots;
        SUnit->crp.prm.Roots = SUnit->crp.prm.Roots->next;
        free(head);
    }
    free(SUnit->crp.prm.Roots);
    SUnit->crp.prm.Roots = NULL;

    while (SUnit->crp.prm.Leaves)
    {
        head = SUnit->crp.prm.Leaves;
        SUnit->crp.prm.Leaves = SUnit->crp.prm.Leaves->next;
        free(head);
    }
    free(SUnit->crp.prm.Leaves);
    SUnit->crp.prm.Leaves = NULL;

    while (SUnit->crp.prm.Stems)
    {
        head = SUnit->crp.prm.Stems;
        SUnit->crp.prm.Stems = SUnit->crp.prm.Stems->next;
        free(head);
    }
    free(SUnit->crp.prm.Stems);
    SUnit->crp.prm.Stems = NULL;

    while (SUnit->crp.prm.Storage)
    {
        head = SUnit->crp.prm.Storage;
        SUnit->crp.prm.Storage = SUnit->crp.prm.Storage->next;
        free(head);
    }
    free(SUnit->crp.prm.Storage);
    SUnit->crp.prm.Storage = NULL;

    while (SUnit->crp.prm.DeathRateStems)
    {
        head = SUnit->crp.prm.DeathRateStems;
        SUnit->crp.prm.DeathRateStems = SUnit->crp.prm.DeathRateStems->next;
        free(head);
    }
    free(SUnit->crp.prm.DeathRateStems);
    SUnit->crp.prm.DeathRateStems = NULL;

    while (SUnit->crp.prm.DeathRateRoots)
    {
        head = SUnit->crp.prm.DeathRateRoots;
        SUnit->crp.prm.DeathRateRoots = SUnit->crp.prm.DeathRateRoots->next;
        free(head);
    }
    free(SUnit->crp.prm.DeathRateRoots);
    SUnit->crp.prm.DeathRateRoots = NULL;

    while (SUnit->crp.prm.N_MaxLeaves)
    {
        head = SUnit->crp.prm.N_MaxLeaves;
        SUnit->crp.prm.N_MaxLeaves = SUnit->crp.prm.N_MaxLeaves->next;
        free(head);
    }
    free(SUnit->crp.prm.N_MaxLeaves);
    SUnit->crp.prm.N_MaxLeaves = NULL;

    while (SUnit->crp.prm.P_MaxLeaves)
    {
        head = SUnit->crp.prm.P_MaxLeaves;
        SUnit->crp.prm.P_MaxLeaves = SUnit->crp.prm.P_MaxLeaves->next;
        free(head);
    }
    free(SUnit->crp.prm.P_MaxLeaves);
    SUnit->crp.prm.P_MaxLeaves = NULL;

    while (SUnit->crp.prm.K_MaxLeaves)
    {
        head = SUnit->crp.prm.K_MaxLeaves;
        SUnit->crp.prm.K_MaxLeaves = SUnit->crp.prm.K_MaxLeaves->next;
        free(head);
    }
    free(SUnit->crp.prm.K_MaxLeaves);
    SUnit->crp.prm.K_MaxLeaves = NULL;

    while (SUnit->soil.VolumetricSoilMoisture)
    {
        head = SUnit->soil.VolumetricSoilMoisture;
        SUnit->soil.VolumetricSoilMoisture = SUnit->soil.VolumetricSoilMoisture->next;
        free(head);
    }
    free(SUnit->soil.VolumetricSoilMoisture);
    SUnit->soil.VolumetricSoilMoisture = NULL;

    while (SUnit->soil.HydraulicConductivity)
    {
        head = SUnit->soil.HydraulicConductivity;
        SUnit->soil.HydraulicConductivity = SUnit->soil.HydraulicConductivity->next;
        free(head);
    }
    free(SUnit->soil.HydraulicConductivity);
    SUnit->soil.HydraulicConductivity = NULL;

    while (SUnit->mng.N_Fert_table)
    {
        head_D = SUnit->mng.N_Fert_table;
        SUnit->mng.N_Fert_table = SUnit->mng.N_Fert_table->next;
        free(head_D);
    }
    free(SUnit->mng.N_Fert_table);
    SUnit->mng.N_Fert_table = NULL;

    while (SUnit->mng.P_Fert_table)
    {
        head_D = SUnit->mng.P_Fert_table;
        SUnit->mng.P_Fert_table = SUnit->mng.P_Fert_table->next;
        free(head_D);
    }
    free(SUnit->mng.P_Fert_table);
    SUnit->mng.P_Fert_table = NULL;

    while (SUnit->mng.K_Fert_table)
    {
        head_D = SUnit->mng.K_Fert_table;
        SUnit->mng.K_Fert_table = SUnit->mng.K_Fert_table->next;
        free(head_D);
    }
    free(SUnit->mng.K_Fert_table);
    SUnit->mng.K_Fert_table = NULL;

    while (SUnit->mng.Irrigation)
    {
        head_D = SUnit->mng.Irrigation;
        SUnit->mng.Irrigation = SUnit->mng.Irrigation->next;
        free(head_D);
    }
    free(SUnit->mng.Irrigation);
    SUnit->mng.Irrigation = NULL;

    while (SUnit->ste.NotInfTB)
    {
        head = SUnit->ste.NotInfTB;
        SUnit->ste.NotInfTB = SUnit->ste.NotInfTB->next;
        free(head);
    }
    free(SUnit->ste.NotInfTB);
    SUnit->ste.NotInfTB = NULL;

    /* Free the leaves of this node. Loop until the last element in the */
    /* list and free each node */
    while (SUnit->crp.LeaveProperties)
    {
        LeaveProperties = SUnit->crp.LeaveProperties;
        SUnit->crp.LeaveProperties = SUnit->crp.LeaveProperties->next;

        free(LeaveProperties);
        LeaveProperties = NULL;
    }

    /* Free the last node */
    free(SUnit->crp.LeaveProperties);

    /* Set the adddress to NULL*/
    SUnit->crp.LeaveProperties = NULL;
}