#include "configuration.h"
#include "penman.h"
#include "simulation.h"
#include "wofost.h"

Config *Configuration;
NetCDFMeta WeatherMetas[WEATHER_NTYPES];
NetCDFMeta *OutputMetas[OUTPUT_NTYPES];

struct tm Start;
struct tm End;

size_t DomainSize;
size_t DomainShape[NR_DOMAIN_DIMENSIONS];
double *Latitudes;
double *Longitudes;
double Resolution;

size_t CropSize;
size_t *ActiveSize;
size_t **ActiveIndex;

DomUnit *DomGrid;
SimUnit **SimGrid;
float *WeatherData;
float *OutputFloatData;
int *OutputIntData;

float AtmosphTransm;
float AngotRadiation;
float Daylength;
float PARDaylength;
float SinLD;
float CosLD;
float DiffRadPP;
float DSinBE;

Etp Penman;
EVP Evtra;

Plant *Crop;
Location *Loc;
Management *Mng;
Weather *Meteo;
Field *Site;
Soil *WatBal;

float Step;
time_t CurrentTime;
size_t CurrentStep;
DomUnit *DUnit;
SimUnit *SUnit;
bool Standalone;
float Temp;
float DayTemp;