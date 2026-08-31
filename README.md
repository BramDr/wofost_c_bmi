# wofost_c_bmi
The WOFOST crop model (c-version; https://github.com/isupit/wofost_c) with Basic Model Interface (BMI; https://github.com/csdms/bmi) implementation.

## Framework changes

### Reorganization
c and header files are now organized into their own directories (`src/` and `include/`) and subdirectories. Code related to the WOFOST crop model (`wofost/` subdirectory) is now seperated from code related to the simulation setup (`simulation/` subdirectory) and code related to the BMI interface (`bmi/` subdirectory).

### Builds
wofost can now be build through
1) a `makefile` (for a bmi-compliant standalone executable)
2) a `CMakeLists.txt` file (for a bmi-compliant standalone executable)
3) a `babel_wofost.toml` (for a bmi-compliant python module)

### Simulation
Each simulation can now handle simulating multiple crops at the same time and only within each crops mask (i.e., the crops active grid cells). Weather information is still read for the entire grid, but this information is shared between crops that are active within the same grid cell.

### BMI
All relevant BMI methods have been implemented. Input/output variable exchange for specific crops is done by prefixing the crop name to the variable name.

## WOFOST changes

### Units
Added weather units support to specify and automatically convert values.

### Options
Added IGNORE_NUTRIENT_STRESS options.


## WOFOST bug-fixes

### Watfd.c
RINPRE can be uninitialized in a specific brach
```
WatBal->rt.Infiltration = 
            (1. - Site->NotInfiltrating * Afgen(Site->NotInfTB, &Rain[0][Lat][Lon])) * Rain[0][Lat][Lon] + WatBal->rt.Irrigation + WatBal->st.SurfaceStorage / Step;
```
```
RINPRE = WatBal->rt.Infiltration =
    (1. - Site->NotInfiltrating * Afgen(Site->NotInfTB, &Meteo->Rain)) *
        Meteo->Rain + WatBal->rt.Irrigation + WatBal->st.SurfaceStorage / Step;
```

### Evtra.c
The DaysOxygenStress postfix increment is immediately clobbered by the assignment.
```
DaysOxygenStress = min(DaysOxygenStress++, 4.)
```
becomes
```
Crop->DaysOxygenStress++;
Crop->DaysOxygenStress = min(Crop->DaysOxygenStress, 4.);
```

### Leaves.c
Added null guard while loop walking LeaveProperties
```
while(Death > Crop->LeaveProperties->weight)
```
becomes
```
while (Crop->LeaveProperties != NULL && Death > Crop->LeaveProperties->weight)
```

### Fillvar.c
Avoid writing element out of range
```
for (i = 0; i <= NR_VARIABLES_SOIL; i++)
```
becomes
```
for (i = 0; i < NR_VARIABLES_SOIL; i++)
```

### Cropdata.c
Added table null checks
```
if (Table[0] == NULL)
    ERR("VERNRTB table must be specified in file %s when vernalization is used (IDSL >= 2).", cropfile);
```
```
if (Table[i] == NULL)
    ERR("Missing required crop table (index %d) in file %s.", i, cropfile);
```