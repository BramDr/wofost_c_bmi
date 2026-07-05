#include <stdio.h>
#include <time.h>
#include "wofost.h"

/* ---------------------------------------------------------------------------*/
/*  function int IfSowing    ()                                               */
/*  Purpose: Checks whether sowing has occurred. Note that if the emergence   */
/*           flag is set to 1 the crop simulation starts the next day. If it  */
/*           is set to 0 the Emergence date has to be established.            */
/* ---------------------------------------------------------------------------*/

void IfSowing(const struct tm start)
{
    struct tm Current = *gmtime(&CurrentTime);
    if (Current.tm_mon == start.tm_mon &&
        Current.tm_mday == start.tm_mday)
    {
        Crop->Sowing = 1;
    }
}