#include <Windows.h>
#include "WindowInput\WindowGlobals.h"
#include "WindowInput\WindowInputInitializer.h"
#include "WindowInput\RunningState.h"
#include "Graphics\GraphicsSystem.h"
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{

	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	
	InitializeWindowAndInput("DirectX11 Interface Test", 1280, 720);
	GraphicsSystem graphics;
	graphics.Init(ghMainWnd);
	graphics.LoadCompiledShader("VertexShader.cso", D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER);
	graphics.LoadCompiledShader("PixelShader.cso", D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER);
	graphics.LoadTexture("Textures/teapot_texture.png");

	StartRunning();
	while(IsRunning())
	{
		UpdateWindowAndInput();
		graphics.Update();
	}

	graphics.DeInit();

	CoUninitialize();
	
	return 0;
}