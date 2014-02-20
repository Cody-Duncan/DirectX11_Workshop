#include "WindowInputInitializer.h"

/// <summary>
/// Initializes the window with given parameters. Initialized input state. 
/// Hooks Input_WndProc for InputState to the WndProc for the generated window.
/// </summary>
/// <remarks>
///     HINSTANCE and HWND stored in WindowGlobals.
///     The state of the keyboard and mouse can be read via gINPUTSTATE in InputState.h
/// </remarks>
/// <param name="windowTitle">The window title.</param>
/// <param name="width">The width.</param>
/// <param name="height">The height.</param>
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