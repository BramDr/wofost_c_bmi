#include "time_utils.h"

int leap_year(int year) {
  if ((year % 400 == 0) || ((year % 100 != 0) && (year % 4 == 0)))
    return 366;
  else
    return 365;
}

static time_t timegm_fallback(struct tm *tm) {
  struct tm copy = *tm;
  time_t local = mktime(&copy);
  if (local == (time_t)-1)
    return (time_t)-1;

  struct tm *utc_back = gmtime(&local);
  if (!utc_back)
    return (time_t)-1;

  time_t corrected = local + (local - mktime(utc_back));
  return corrected;
}

time_t timegm_portable(struct tm *tm) {
#if defined(__GLIBC__) && defined(_GNU_SOURCE)
  return timegm(tm);
#else
  return timegm_fallback(tm);
#endif
}