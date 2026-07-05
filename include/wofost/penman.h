
#ifndef PENMAN_H
#define PENMAN_H

typedef struct ETP {
  float E0;
  float ES0;
  float ET0;
} Etp;

typedef struct EVP {
  float MaxEvapWater;
  float MaxEvapSoil;
  float MaxTranspiration;
} EVP;

extern Etp Penman;
extern EVP Evtra;

extern void CalcPenman(void);
extern void CalcPenmanMonteith(void);

#endif // PENMAN_H
