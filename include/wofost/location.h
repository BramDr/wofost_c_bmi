#ifndef POSITION_H
#define POSITION_H

typedef struct LOCATION {
  float Latitude;
  float Longitude;
  float Altitude;
  float AngstA;
  float AngstB;
} Location;

extern Location *Loc;

#endif // POSITION_H