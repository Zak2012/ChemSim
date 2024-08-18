#include "Time.hpp"

#ifdef _WIN32
#undef WINVER
#define WINVER NTDDI_WIN10_19H1

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <windows.h>
void StartAccurateSleep()
{
    timeBeginPeriod(1);
}
#else
void StartAccurateSleep(){}
#endif