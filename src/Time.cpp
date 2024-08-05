#include "Time.hpp"

#ifdef _WIN32
#include <windows.h>
void StartAccurateSleep()
{
    timeBeginPeriod(1);
}
#else
void StartAccurateSleep(){}
#endif