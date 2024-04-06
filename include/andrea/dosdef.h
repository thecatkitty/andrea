#ifndef _ANDREA_DOSDEF_H_
#define _ANDREA_DOSDEF_H_

#include <stddef.h>
#include <stdint.h>

#ifndef far
#ifdef EDITING
#define far
#else // EDITING
#define far __far
#endif // EDITING
#endif // far

#endif // _ANDREA_DOSDEF_H_
