#include "WindowInput\RunningState.h"

bool IsRunningBool = false;

void StartRunning()
{
    IsRunningBool = true;
}
bool IsRunning()
{
    return IsRunningBool;
}
void StopRunning()
{
    IsRunningBool = false;
}