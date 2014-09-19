#include <Windows.h>
#include "WindowInput\WindowGlobals.h"
#include "WindowInput\WindowInputInitializer.h"
#include "WindowInput\RunningState.h"
#include "Graphics\GraphicsSystem.h"
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	
	InitializeWindowAndInput("DirectX11 Interface Test", 1280, 720);
	GraphicsSystem graphics;
	graphics.Init(ghMainWnd);
	graphics.LoadSourceShader("Shaders/VertexShader.hlsl", "main", "vs_5_0");
	graphics.LoadSourceShader("Shaders / PixelShader.hlsl", "main", "ps_5_0");

	StartRunning();
	while(IsRunning())
	{
		UpdateWindowAndInput();
		graphics.Update();
	}

	graphics.DeInit();
	
	return 0;
}