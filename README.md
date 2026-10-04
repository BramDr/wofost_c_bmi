# wofost_c_bmi
The WOFOST crop model (c-version; https://github.com/isupit/wofost_c) with Basic Model Interface (BMI; https://github.com/csdms/bmi) implementation.

## Getting started

### Requirements
Only [pixi](https://pixi.sh) is required. It installs the compiler, CMake, the C dependencies (netCDF, bmi-c) and the Python tools (Python 3.14, babelizer) from conda-forge into a project-local environment (`.pixi/envs/default`), and runs the build steps. No system packages or conda activation are needed. `pixi.lock` pins the exact package versions; `pixi update` refreshes them.

### Build and install
```
pixi run install-c
pixi run install-py
pixi run test-install-c
pixi run test-install-py
```
builds the C library and executable, installs them into the pixi environment, generates and installs the Python module, and checks that it can be imported. The individual steps are pixi tasks (`pixi task list`) defined in `pixi.toml`.

## Framework changes

### Reorganization
c and header files are now organized into their own directories (`src/` and `include/`) and subdirectories. Code related to the WOFOST crop model (`wofost/` subdirectory) is now seperated from code related to the simulation setup (`simulation/` subdirectory) and code related to the BMI interface (`bmi/` subdirectory).

### Builds
The original `makefile` has been replaced by CMake (`CMakeLists.txt`), which is the only supported build system. It produces
1) a bmi-compliant shared library (`libbmiwofost.so`)
2) a bmi-compliant standalone executable (`wofost`)

The shared library is wrapped into a bmi-compliant python module (`pymt_wofost`) with babelizer through `babel.toml`.
Dependencies and build steps are managed with pixi (`pixi.toml`); see [Getting started](#getting-started).

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