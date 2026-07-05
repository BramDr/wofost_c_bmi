#ifndef OUTPUT_H
#define OUTPUT_H

#include "defs.h"
#include <netcdf.h>

typedef struct OUTPUTVAR
{
    const char name[MAX_STRING];
    const nc_type type;
    const char units[MAX_STRING];
    const char standard_name[MAX_STRING];
    const char long_name[MAX_STRING];
} OutputVar;

static const OutputVar LAT_VAR = {
    "latitude",
    NC_DOUBLE,
    "degrees",
    "latitude",
    "latitude of the grid cell center",
};

static const OutputVar LON_VAR = {
    "longitude",
    NC_DOUBLE,
    "degrees",
    "longitude",
    "longitude of the grid cell center",
};

static const OutputVar TIME_VAR = {
    "time",
    NC_INT,
    "days since 1970-01-01",
    "time",
    "time in days since 1970-01-01",
};

static const OutputVar OUTPUT_VARS[] = {
    {
        "growth_day",
        NC_INT,
        "days",
        "growth day",
        "growth day since emergence",
    },
    {
        "development",
        NC_FLOAT,
        "-",
        "development",
        "development stage between 0 (emergence), 1 (flowering), and 2 (maturity)",
    },
    {
        "root_biomass",
        NC_FLOAT,
        "kg ha-1",
        "root biomass",
        "biomass in root organs",
    },
    {
        "stem_biomass",
        NC_FLOAT,
        "kg ha-1",
        "stem biomass",
        "biomass in stem organs",
    },
    {
        "leaf_biomass",
        NC_FLOAT,
        "kg ha-1",
        "leaf biomass",
        "biomass in leaf organs",
    },
    {
        "storage_biomass",
        NC_FLOAT,
        "kg ha-1",
        "storage biomass",
        "biomass in storage organs",
    },
    {
        "root_depth",
        NC_FLOAT,
        "cm",
        "root depth",
        "root depth",
    },
    {
        "leaf_area_index",
        NC_FLOAT,
        "m2 m-2",
        "leaf area index",
        "leaf area index",
    },
    {
        "stress",
        NC_FLOAT,
        "-",
        "stress",
        "total stress (water, heat, and nutrient)",
    },
    {
        "water_stress",
        NC_FLOAT,
        "-",
        "water stress",
        "water stress",
    },
    {
        "heat_stress",
        NC_FLOAT,
        "-",
        "heat stress",
        "heat stress",
    },
    {
        "nutrient_stress",
        NC_FLOAT,
        "-",
        "nutrient stress",
        "nutrient stress",
    },
    {
        "root_dead",
        NC_FLOAT,
        "kg ha-1",
        "root dead biomass",
        "dead biomass from root organs",
    },
    {
        "stem_dead",
        NC_FLOAT,
        "kg ha-1",
        "stem dead biomass",
        "dead biomass from stem organs",
    },
    {
        "leaf_dead",
        NC_FLOAT,
        "kg ha-1",
        "leaf dead biomass",
        "dead biomass from leaf organs",
    },
    {
        "root_N_content",
        NC_FLOAT,
        "kg ha-1",
        "root nitrogen content",
        "Nitrogen content in root organs",
    },
    {
        "stem_N_content",
        NC_FLOAT,
        "kg ha-1",
        "stem nitrogen content",
        "Nitrogen content in stem organs",
    },
    {
        "leaf_N_content",
        NC_FLOAT,
        "kg ha-1",
        "leaf nitrogen content",
        "Nitrogen content in leaf organs",
    },
    {
        "storage_N_content",
        NC_FLOAT,
        "kg ha-1",
        "storage nitrogen content",
        "Nitrogen content in storage organs",
    },
    {
        "root_P_content",
        NC_FLOAT,
        "kg ha-1",
        "root phosporus content",
        "Phosporus content in root organs",
    },
    {
        "stem_P_content",
        NC_FLOAT,
        "kg ha-1",
        "stem phosporus content",
        "Phosporus content in stem organs",
    },
    {
        "leaf_P_content",
        NC_FLOAT,
        "kg ha-1",
        "leaf phosporus content",
        "Phosporus content in leaf organs",
    },
    {
        "storage_P_content",
        NC_FLOAT,
        "kg ha-1",
        "storage phosporus content",
        "Phosporus content in storage organs",
    },
    {
        "root_K_content",
        NC_FLOAT,
        "kg ha-1",
        "root potassium content",
        "Potassium content in root organs",
    },
    {
        "stem_K_content",
        NC_FLOAT,
        "kg ha-1",
        "stem potassium content",
        "Potassium content in stem organs",
    },
    {
        "leaf_K_content",
        NC_FLOAT,
        "kg ha-1",
        "leaf potassium content",
        "Potassium content in leaf organs",
    },
    {
        "storage_K_content",
        NC_FLOAT,
        "kg ha-1",
        "storage potassium content",
        "Potassium content in storage organs",
    },
};
static const int N_OUTPUT_VARS = sizeof(OUTPUT_VARS) / sizeof(OUTPUT_VARS[0]);

#endif // OUTPUT_H