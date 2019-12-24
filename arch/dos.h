#ifndef DOS_H_
#define DOS_H_

#ifdef WIN32
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static intptr_t find_last;
#pragma GCC diagnostic pop
#define _dos_findfirst(spec, x, data) ((find_last = _findfirst(spec, data)) == -1)
#define _dos_findnext(data) _findnext(find_last, data)
#define find_t _finddata_t
#else
//#define _dos_findfirst(spec, x, data) (spec,x,data,1)
//#define _dos_findnext(data) (data,1)
struct find_t { const char name[13]; };
typedef struct find_t find_t;
int _dos_findfirst(const char *spec, int attr, find_t *data);
int _dos_findnext(find_t *data);
#define _A_NORMAL 0
#define EZERO 0
#endif

#endif
