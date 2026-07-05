#include <stdio.h>
#include <time.h>
#include "wofost.h"

/* ---------------------------------------------------------------------------*/
/*  function int IfSowing    ()                                               */
/*  Purpose: Checks whether sowing has occurred. Note that if the emergence   */
/*           flag is set to 1 the crop simulation starts the next day. If it  */
/*           is set to 0 the Emergence date has to be established.            */
/* ---------------------------------------------------------------------------*/

void IfSowing(const int start)
{
    struct tm Current = *gmtime(&CurrentTime);
    if (Current.tm_yday == start)
    {
        Crop->Sowing = 1;
    }
}