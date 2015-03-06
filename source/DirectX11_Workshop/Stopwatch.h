/* Stopwatch class
   Author: Giovanni Dicanio with modifications by Cody Duncan
   Source: http://msmvps.com/blogs/gdicanio/archive/2010/01/11/measuring-performance-of-c-code.aspx
   */

#pragma once
#include <Windows.h>
#include <chrono>

class Stopwatch
{
public:
	using Clock     = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Duration  = std::chrono::nanoseconds;

	Stopwatch(bool SingleThread = true);

	void Start();
	void Stop();

	double ElapsedTimeSeconds() const;
	double ElapsedTimeMilliSeconds() const;
	Duration ElapsedTime() const;

private:

	TimePoint m_startCount;
	Duration  m_elapsedTime;

private:
	// *** Ban copy ***
	Stopwatch& operator=(const Stopwatch&) = delete;
	Stopwatch(const Stopwatch&) = delete;
};
