
#include "Timer\Stopwatch.h"


Stopwatch::Stopwatch(bool singleThread) : m_startCount(), m_elapsedTime(0)
{
	if(singleThread)
	{
		// Does some initialization to get consistent results for all tests:
		// Confine the test to run on a single processor
		SetThreadAffinityMask(GetCurrentThread(), 1);
		SetThreadIdealProcessor(GetCurrentThread(), 0);
		Sleep(1); //reset the thread
	}
}

/// <summary>
/// Starts timing. (to be called before the block of code to measure)
/// </summary>
void Stopwatch::Start()
{
	m_elapsedTime = Duration(0);
	m_startCount = std::chrono::high_resolution_clock::now();
}

/// <summary>
/// Stops timing. (to be called after the block of code to measure)
/// </summary>
void Stopwatch::Stop()
{
	TimePoint stopCount = std::chrono::high_resolution_clock::now();
	m_elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(stopCount - m_startCount);
	m_startCount = TimePoint();
}


/// <summary>
/// Calculates the elapsed time between Start() and Stop() in seconds.
/// </summary>
/// <returns>total elapsed time (in seconds) in Start-Stop interval.</returns>
double Stopwatch::ElapsedTimeSeconds() const
{
	return std::chrono::duration_cast<std::chrono::duration<double>>(m_elapsedTime).count();
}


/// <summary>
/// Calculates the elapsed time between Start() and Stop() in milliseconds.
/// </summary>
/// <returns>total elapsed time (in milliseconds) in Start-Stop interval.</returns>
double Stopwatch::ElapsedTimeMilliSeconds() const
{
	return std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(m_elapsedTime).count();
}

/// <summary>
/// Returns the elapsed time between Start() and Stop() as an std::chrono duration. 
/// </summary>
/// <returns></returns>
Stopwatch::Duration Stopwatch::ElapsedTime() const
{
	return m_elapsedTime;
}
