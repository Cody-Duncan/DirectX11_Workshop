#include "OSHighResTimer.h"

LARGE_INTEGER frequencyTemp;
/// <summary>
/// Reads the high-resolution timer frequency.
/// </summary>
/// <remarks>
/// The high frequency counter need not be tied to the CPU frequency at all. It will only resemble the CPU frequency. 
/// This function actually uses the TSC (TimeStampCounter) underneath. As the TSC is generally unreliable on multi-core systems 
/// it tends not to be used. When the TSC is not used the ACPI Power Management Timer (pmtimer) may be used. You can tell if your 
/// system uses the ACPI PMT by checking if QueryPerformanceFrequency returns the signature value of 3,579,545 (ie 3.57MHz). 
/// If you see a value around 1.19Mhz then your system is using the old 8245 PIT chip. Otherwise you should see a value approximately 
/// that of your CPU frequency (modulo any speed throttling or power-management that might be in effect.) 
/// If you have a newer system with an invariant TSC (ie constant frequency TSC) then that is the frequency 
/// that will be returned (if Windows uses it). Again this is not necessarily the CPU frequency.
/// </remarks>
/// <returns></returns>
__int64 readHiResTimerFrequency()
{
        #ifdef WIN32   // Windows system specific
                QueryPerformanceFrequency(&frequencyTemp);
                return frequencyTemp.QuadPart;
        #else          // Unix based system specific
        
        #endif
}

LARGE_INTEGER timerTemp;
/// <summary>
/// Reads the high-resolution timer's current value (time).
/// </summary>
/// <returns></returns>
__int64 readHiResTimer()
{
        #ifdef WIN32   // Windows system specific
                QueryPerformanceCounter(&timerTemp);
                return timerTemp.QuadPart;
        #else          // Unix based system specific
        
        #endif
}