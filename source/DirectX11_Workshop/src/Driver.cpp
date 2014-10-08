#include <Windows.h>
#include "WindowInput\WindowGlobals.h"
#include "WindowInput\WindowInputInitializer.h"
#include "WindowInput\RunningState.h"
#include "Graphics\GraphicsSystem.h"

/* 
	Note: A Message like "The application was unable to start correctly (0xc000007b). Click Ok to close the application." means that it cannot
	find a .dll file. Probably d3dcompiler_47.dll. Find the dll and put it in the output directory.
	
	Referenced dlls:
		d3dcompiler_47.dll  - C:\Program Files (x86)\Windows Kits\8.1\bin\x86\

	Setting PATH to Dlls
		If you don't want to include dll's in the output directory, they need to be added to the application's environment, the PATH variable.
		More info here: http://msdn.microsoft.com/en-us/library/windows/desktop/ms682586(v=vs.85).aspx
	
	Debugging PATH variable
		Setting the path for debugging can be done via:
			Project Properties -> Configuration Properties -> General -> Debugging -> (fill this field) Environment
	
	Post-build event
		For convenience, any dll files added to the lib directory will be copied to the output directory. This makes it so the output directory
		can be packed up, moved, unpacked, and run with no setup (as long as assets are copied too). 
		See the post-build event of project properties.
*/

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