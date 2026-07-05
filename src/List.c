#include "wofost.h"

/* ---------------------------------------------------------------------------*/
/*  function List()                                                           */
/*  Purpose: Get the value of a user provided input table                     */
/* ---------------------------------------------------------------------------*/

float List(TABLE_D *Table)
{
    struct tm Current = *gmtime(&CurrentTime);
    while (Table)
    {
        if (Current.tm_mon == Table->month - 1 &&
            Current.tm_mday == Table->day)
        {
            return Table->amount;
        }
        Table = Table->next;
    }

    return 0.;
}
