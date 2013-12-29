#pragma once

#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

#ifndef HIGHRESTIMER
#define HIGHRESTIMER

__int64 readHiResTimerFrequency();
__int64 readHiResTimer();

#endif