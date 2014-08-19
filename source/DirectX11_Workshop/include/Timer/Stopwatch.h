/* Stopwatch class 
   Author: Giovanni Dicanio with modifications by Cody Duncan
   Source: http://msmvps.com/blogs/gdicanio/archive/2010/01/11/measuring-performance-of-c-code.aspx
*/

#pragma once
#include <Windows.h>
#include "OSHighResTimer.h"

class Stopwatch
{
public:
    Stopwatch();

    void Start();
    void Stop();

    double ElapsedTimeSec() const;
    double ElapsedTimeMilliSec() const;

private:
    LONGLONG m_startCount;
    double m_elapsedTimeSec;
    LONGLONG Counter() const;
    LONGLONG Frequency() const;

private:
    // *** Ban copy ***
    Stopwatch(const Stopwatch &);
    Stopwatch & operator=(const Stopwatch &);
};
