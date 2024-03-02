#ifndef _ANDREA_H_
#define _ANDREA_H_

#include <stdint.h>

#ifndef far
#ifdef EDITING
#define far
#else // EDITING
#define far __far
#endif // EDITING
#endif // far

typedef enum
{
    ANDREA_SUCCESS = 0,
    ANDREA_ERROR_MISSING_PARAMETER = 256,
    ANDREA_ERROR_INVALID_PARAMETER = 257,
} andrea_status;

extern void
andrea_fptoa(char *buffer, const void far *fp);

extern void far *
andrea_atofp(const char *buffer);

#endif
