#include "WindowInput\WindowInputInitializer.h"


void InitializeWindowAndInput(const char* windowTitle, int width, int height)
{
	InitGlobalInput();
	GenerateWindow(Input_WndProc, windowTitle, width, height);
	ActivateWindow();
}

/// <summary>
/// Moves current input state to previous input state.
/// Then updates and dispatches windows messsages, input messages will be set as the new current input state.
/// </summary>
void UpdateWindowAndInput()
{
	gINPUTSTATE->Update();
	UpdateWindowMsgs();
}