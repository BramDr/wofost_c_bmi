#include "configuration.h"
#include <ctype.h>

char *trim(char *str, size_t len)
{
    char *limit = str + len;

    // Trim leading space
    while (str < limit && isspace((unsigned char)*str))
    {
        str++;
    }
    if (str >= limit || *str == '\0')
    {
        return str;
    }

    // Trim trailing space
    while (limit > str && isspace((unsigned char)*(limit - 1)))
    {
        limit--;
    }

    *limit = '\0'; // Null-terminate the trimmed string
    return str;
}
