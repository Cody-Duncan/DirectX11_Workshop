#include "InputState.h"

#include<windowsx.h>

double intput;
InputState* gINPUTSTATE;

InputState::InputState(void)
{
    gINPUTSTATE = this;
}
InputState::~InputState(void)
{
    gINPUTSTATE = nullptr;
}

void InputState::Init()
{
    KeyboardStatePrev.resize(256);
    KeyboardStateNow.resize(256);

    for(unsigned int i = 0; i < 256; ++i)
    {
        KeyboardStatePrev[i] = false;
        KeyboardStateNow[i] = false;
    }
    MousePositionPrevX = 0;
    MousePositionPrevY = 0;
    MousePositionNowX = 0;
    MousePositionNowY = 0;

    MouseStatePrev[MouseButton::Left] = false;
    MouseStatePrev[MouseButton::Right] = false;
    MouseStatePrev[MouseButton::Middle] = false;

    MouseStateNow[MouseButton::Left] = false;
    MouseStateNow[MouseButton::Right] = false;
    MouseStateNow[MouseButton::Middle] = false;
}

static unsigned int bit30Mask = 1<<30;

void InputState::updateKey(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_KEYDOWN:
            if( !(lParam & bit30Mask) )
            {
                KeyboardStateNow[wParam] = true;
                //DebugPrintf("KeyNum: %d ; KeyState:%s \n", (unsigned int)wParam, "true");
            }
            break;
        case WM_KEYUP:
                KeyboardStateNow[wParam] = false;
                //DebugPrintf("KeyNum: %d ; KeyState:%s \n", (unsigned int)wParam, "false" );
            break;
        case WM_LBUTTONDOWN:
            MouseStateNow[Left] = true;
            break;
        case WM_RBUTTONDOWN:
            MouseStateNow[Right] = true;
            break;

        case WM_LBUTTONUP:
            MouseStateNow[Left] = false;
            break;
        case WM_RBUTTONUP:
            MouseStateNow[Right] = false;
            break;
        case WM_MOUSEMOVE:
            UpdateMousePos(lParam);
            break;
    }
}

void InputState::UpdateMousePos(LPARAM lParam)
{
    MousePositionNowX  = GET_X_LPARAM(lParam); 
    MousePositionNowY = GET_Y_LPARAM(lParam);
}

/// <summary>
/// Moves the current state to the previous state.
/// </summary>
void InputState::Update()
{
    KeyboardStatePrev = KeyboardStateNow;
    MousePositionPrevX = MousePositionNowX;
    MousePositionPrevY = MousePositionNowY;
    MouseStatePrev = MouseStateNow;
}

bool InputState::keyDown(WPARAM wParam)
{
    return KeyboardStateNow[wParam];
}

bool InputState::keyHeld(WPARAM wParam)
{
    return KeyboardStatePrev[wParam] && KeyboardStateNow[wParam];
}

bool InputState::keyPressed(WPARAM wParam)
{
    return ! KeyboardStatePrev[wParam] && KeyboardStateNow[wParam];
}

bool InputState::keyUp(WPARAM wParam)
{
    return KeyboardStatePrev[wParam] && ! KeyboardStateNow[wParam];
}

bool InputState::mouseDown(MouseButton button)
{
    return MouseStateNow[button];
}
bool InputState::mouseHeld(MouseButton button)
{
    return MouseStatePrev[button] && MouseStateNow[button];
}
bool InputState::mousePressed(MouseButton button)
{
    return ! MouseStatePrev[button] && MouseStateNow[button];
}
bool InputState::mouseUp(MouseButton button)
{
    return MouseStatePrev[button] && ! MouseStateNow[button];
}

void InitGlobalInput() 
{ 
    gINPUTSTATE = new InputState();
    gINPUTSTATE->Init();
}

LRESULT CALLBACK Input_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            gINPUTSTATE->updateKey(msg, wParam, lParam);
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
            gINPUTSTATE->updateKey(msg, wParam, lParam);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}