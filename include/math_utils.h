#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

/* set decimals */
#define roundz(x, d) ((floor(((x) * pow(10, d)) + .5)) / pow(10, d))
#define roundz1(x) ((floor(((x) * 10) + .5)) / 10)
#define roundz2(x) ((floor(((x) * 100) + .5)) / 100)

#endif // MATH_UTILS_H