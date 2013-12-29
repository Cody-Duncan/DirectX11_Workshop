#include "stdafx.h"
#include "Stopwatch.h"


Stopwatch::Stopwatch() : m_startCount(0), m_elapsedTimeSec(0.0)
{
    // Does some initialization to get consistent results for all tests:
    // Confine the test to run on a single processor,
    // to get consistent results for all tests.
    SetThreadAffinityMask(GetCurrentThread(), 1);
    SetThreadIdealProcessor(GetCurrentThread(), 0);
    Sleep(1);
}

/// <summary>
/// Starts timing. (to be called before the block of code to measure)
/// </summary>
void Stopwatch::Start()
{
    m_elapsedTimeSec = 0.0;
    m_startCount = Counter();
}

/// <summary>
/// Stops timing. (to be called after the block of code to measure)
/// </summary>
void Stopwatch::Stop()
{
    LONGLONG stopCount = Counter();
    // time is measured in seconds
    m_elapsedTimeSec = (stopCount - m_startCount) * 1.0 / Frequency();
    m_startCount = 0;
}


/// <summary>
/// Calculates the elapsed time between Start() and Stop() in seconds.
/// </summary>
/// <returns>total elapsed time (in seconds) in Start-Stop interval.</returns>
double Stopwatch::ElapsedTimeSec() const
{
    return m_elapsedTimeSec;
}


/// <summary>
/// Calculates the elapsed time between Start() and Stop() in milliseconds.
/// </summary>
/// <returns>total elapsed time (in milliseconds) in Start-Stop interval.</returns>
double Stopwatch::ElapsedTimeMilliSec() const
{
    return m_elapsedTimeSec * 1000.0;
}


/// <summary>
/// Queries the high-resolution counter on the machine.
/// </summary>
/// <returns>current value of high-resolution counter.</returns>
LONGLONG Stopwatch::Counter() const
{
    return readHiResTimer();
}


/// <summary>
/// Queries the frequency (in counts per second) of the high-resolution counter
/// </summary>
/// <returns>frequency of the high-resolution counter.</returns>
LONGLONG Stopwatch::Frequency() const
{
    return readHiResTimerFrequency();
}