## Framework changes

### Units
Added weather units support to specify and automatically convert values.

## WOFOST changes

### Options
Added IGNORE_NUTRIENT_STRESS and IGNORE_SOIL_MOISTURE options (instead of commenting out code).


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