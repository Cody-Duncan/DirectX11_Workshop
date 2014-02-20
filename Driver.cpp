#include <Windows.h>
#include "WindowInput\WindowInputInitializer.h"
#include "WindowInput\RunningState.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    InitializeWindowAndInput("DirectX11 Interface Test", 1280, 720);

    StartRunning();
    while(IsRunning())
    {
        UpdateWindowAndInput();
    }

    return 0;
}