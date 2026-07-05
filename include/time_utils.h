#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <time.h>

extern int leap_year(int year);
extern time_t timegm_portable(struct tm *tm);

#endif // TIME_UTILS_H