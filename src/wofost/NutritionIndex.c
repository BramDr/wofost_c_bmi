#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wofost.h"

/* --------------------------------------------------------------------*/
/*  function NutritionINDX()                                           */
/*  Purpose: To compute N,P,K Nutrition Index (-)                      */
/* --------------------------------------------------------------------*/
void NutritionINDX(void)
{
    float VegetativeMass;

    float N_opt_veg = 0.;
    float P_opt_veg = 0.;
    float K_opt_veg = 0.;

    float N_Veg = 0.;
    float P_Veg = 0.;
    float K_Veg = 0.;

    float N_res = 0.;
    float P_res = 0.;
    float K_res = 0.;

    /* Total vegetative living above-ground biomass (kg DM ha-1)     */
    VegetativeMass = Crop->st.leaves + Crop->st.stems;

    if (VegetativeMass > 0.)
    {
        /* N,P,K concentration in total vegetative living per */
        /* kg above-ground biomass  (kg N,P,K kg-1 DM)        */
        N_Veg = (Crop->N_st.leaves + Crop->N_st.stems) / VegetativeMass;
        P_Veg = (Crop->P_st.leaves + Crop->P_st.stems) / VegetativeMass;
        K_Veg = (Crop->K_st.leaves + Crop->K_st.stems) / VegetativeMass;

        /* Residual N,P,K concentration in total vegetative living */
        /* above-ground biomass  (kg N,P,K kg-1 DM)                */
        N_res = (Crop->st.leaves * Crop->prm.N_ResidualFrac_lv + Crop->st.stems * Crop->prm.N_ResidualFrac_st) / VegetativeMass;
        P_res = (Crop->st.leaves * Crop->prm.P_ResidualFrac_lv + Crop->st.stems * Crop->prm.P_ResidualFrac_st) / VegetativeMass;
        K_res = (Crop->st.leaves * Crop->prm.K_ResidualFrac_lv + Crop->st.stems * Crop->prm.K_ResidualFrac_st) / VegetativeMass;

        N_opt_veg = (Crop->N_st.Optimum_lv + Crop->N_st.Optimum_st) / VegetativeMass;
        P_opt_veg = (Crop->P_st.Optimum_lv + Crop->P_st.Optimum_st) / VegetativeMass;
        K_opt_veg = (Crop->K_st.Optimum_lv + Crop->K_st.Optimum_st) / VegetativeMass;
    }

    float tiny = 0.001;
    if ((N_opt_veg - N_res) > tiny)
    {
        Crop->N_st.Indx = limit(tiny, 1.0, (N_Veg - N_res) / (N_opt_veg - N_res));
    }
    else
    {
        Crop->N_st.Indx = tiny;
    }

    if ((P_opt_veg - P_res) > tiny)
    {
        Crop->P_st.Indx = limit(tiny, 1.0, (P_Veg - P_res) / (P_opt_veg - P_res));
    }
    else
    {
        Crop->P_st.Indx = tiny;
    }

    if ((K_opt_veg - K_res) > tiny)
    {
        Crop->K_st.Indx = limit(tiny, 1.0, (K_Veg - K_res) / (K_opt_veg - K_res));
    }
    else
    {
        Crop->K_st.Indx = tiny;
    }

    // Crop->N_st.Indx = 1.0;
    // Crop->P_st.Indx = 1.0;
    // Crop->K_st.Indx = 1.0;

    Crop->NPK_Indx = (Crop->N_st.Indx < Crop->P_st.Indx) ? Crop->N_st.Indx : Crop->P_st.Indx;
    Crop->NPK_Indx = (Crop->NPK_Indx < Crop->K_st.Indx) ? Crop->NPK_Indx : Crop->K_st.Indx;

    /* Nutrient reduction factor */
    Crop->NutrientStress = limit(0., 1.0, 1. - Crop->prm.NLUE * pow((1.0001 - Crop->NPK_Indx), 2));
}
