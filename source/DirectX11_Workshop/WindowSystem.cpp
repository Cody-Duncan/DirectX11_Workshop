
#include "WindowSystem.h"
#include "WindowGlobals.h"
#include "RunningState.h"
#include <tchar.h>



bool GenerateWindow(WNDPROC WndProc, const char* windowTitle, int width, int height)
{
    static TCHAR szWindowClass[] = _T("shady_tree_app");
    static TCHAR szTitle[] = _T("ShadyTree Test");

    HINSTANCE inst = GetModuleHandle(NULL);

    RECT winSize = {0, 0, width, height};
    AdjustWindowRect(&winSize,			
                    WS_OVERLAPPEDWINDOW,
                    FALSE);	

    WNDCLASSEX  wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW; //redraw if resized or moved that changes height or width
    wcex.lpfnWndProc    = WndProc;                 //pass the WndProc function
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = inst;
    wcex.hIcon          = LoadIcon(inst, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if(!RegisterClassEx(&wcex))
    {
        MessageBox(0, L"RegisterClass FAILED", 0, 0);
        return false;
    }

    ghInstance = inst;

    //Create Window - get an HWND
    ghMainWnd = CreateWindow(
        szWindowClass,          // Registered WNDCLASS instance to use.
        szTitle,                // window title
        WS_OVERLAPPEDWINDOW,    // style flags
        CW_USEDEFAULT,          // x-coordinate
        CW_USEDEFAULT,          // y-coordinate
        winSize.right - winSize.left,  // width
        winSize.bottom - winSize.top,   // height
        0,                      // parent window
        0,                      // menu handle
        inst,                   // app instance
        0                       // extra creation parameters
    ); 

    if(ghMainWnd == 0)
    {
        MessageBox(0, L"CreateWindow FAILED", 0, 0);
        return false;
    }

    return true;
}

void ActivateWindow()	//Activate the game window so it is actually visible
{
    ShowWindow(ghMainWnd, SW_SHOWDEFAULT);
    UpdateWindow(ghMainWnd);
}

void UpdateWindowMsgs()
{
    MSG msg = {0};
    
    while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        if (msg.message == WM_QUIT)
        {
            StopRunning();
        }
    }
}



