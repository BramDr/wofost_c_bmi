#ifndef METEO_H
#define METEO_H

#define TMIN_HISTORY_LENGTH 7

typedef struct WEATHER {
  float Tmin;
  float Tmax;
  float Radiation;
  float Rain;
  float Windspeed;
  float Vapour;
  float Tmin_history[TMIN_HISTORY_LENGTH];
} Weather;

extern Weather *Meteo;

#endif // METEO_H