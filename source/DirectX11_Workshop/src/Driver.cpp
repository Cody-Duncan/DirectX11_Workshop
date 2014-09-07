#include <Windows.h>
#include "WindowInput\WindowGlobals.h"
#include "WindowInput\WindowInputInitializer.h"
#include "WindowInput\RunningState.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\ShaderFactory.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	
	InitializeWindowAndInput("DirectX11 Interface Test", 1280, 720);

	GraphicsSystem graphics;
	graphics.Init(ghMainWnd);

	StartRunning();
	while(IsRunning())
	{
		UpdateWindowAndInput();
		graphics.Update();
	}

	graphics.DeInit();
	
	return 0;
}