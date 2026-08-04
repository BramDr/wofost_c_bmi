#ifndef SIMULATION_H
#define SIMULATION_H

#define NR_DOMAIN_DIMENSIONS 2
#define DOMAIN_DIMENSION_SIZE_MAX 43200 // 30 arc-seconds longitude size

#include "configuration.h"
#include "wofost.h"

/** Time Variables  **/
extern struct tm Start;
extern struct tm End;

/** Domain Variables  **/
extern size_t DomainSize;
extern size_t DomainShape[NR_DOMAIN_DIMENSIONS];
extern double *Latitudes;
extern double *Longitudes;
extern double Resolution;
extern double West, East, North, South;

/** Mask Variables  **/
extern size_t CropSize;
extern size_t *ActiveSize;
extern size_t **ActiveIndex;

/** Simulation Variables  **/
extern DomUnit *DomGrid;
extern SimUnit **SimGrid;
extern float *WeatherData;
extern float *OutputFloatData;
extern int *OutputIntData;

extern void InitializeNetCDFMeta(const char *path, const char *variable_name,
                                 NetCDFMeta *meta);
extern void DeriveNetCDFMetaSpace(const double west, const double east,
                                  const double south, const double north,
                                  const size_t shape[NR_DOMAIN_DIMENSIONS],
                                  NetCDFMeta *meta);
extern void DeriveNetCDFMetaTime(const time_t start, const time_t end,
                                 NetCDFMeta *meta);
extern void ReadNetCDFMetaFloat(const NetCDFMeta *meta, const float fill_value,
                                float data[], size_t time_index);
extern void ReadNetCDFMetaInt(const NetCDFMeta *meta, const int fill_value,
                              int data[], size_t time_index);
extern void FreeNetCDFMeta(NetCDFMeta *meta);

extern void InitializeDomainUnits(void);
extern void FinalizeDomainUnits(void);

extern void InitializeSimulationUnits(void);
extern void FinalizeSimulationUnits(void);

extern void InitializeMeteo(void);
extern void UpdateMeteo(void);
extern void FinalizeMeteo(void);

extern void InitializeOutput(void);
extern void UpdateOutput(void);
extern void FinalizeOutput(void);

extern void InitializeSimulation(const char *config_file);
extern void UpdateSimulation(void);
extern void FinalizeSimulation(void);

#endif // SIMULATION_H