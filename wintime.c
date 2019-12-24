#include <windows.h>
#define byte classic_byte
#include "fix.h"
#undef byte
static DWORD start_time;
void timer_init() { start_time = timeGetTime(); }
fix timer_get_fixed_seconds() { return ((timeGetTime() - start_time) << 13) / 125; }
fix timer_get_approx_seconds() { return timer_get_fixed_seconds(); }
