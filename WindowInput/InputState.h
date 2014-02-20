#pragma once

#include <Windows.h>
#include <vector>
#include <unordered_map>


enum MouseButton
{
    Left,
    Right,
    Middle
};

class InputState
{
public:
    InputState(void);
    ~InputState(void);

    void Init();
    void updateKey(UINT msg, WPARAM wParam, LPARAM lParam);
    void Update();

    bool keyDown(WPARAM wParam);
    bool keyHeld(WPARAM wParam);
    bool keyPressed(WPARAM wParam);
    bool keyUp(WPARAM wParam);

    bool mouseDown(MouseButton button);
    bool mouseHeld(MouseButton button);
    bool mousePressed(MouseButton button);
    bool mouseUp(MouseButton button);

    std::vector<bool> KeyboardStateNow;
    std::unordered_map<MouseButton, bool> MouseStateNow;
    int MousePositionPrevX;
    int MousePositionPrevY;

    std::vector<bool> KeyboardStatePrev;
    std::unordered_map<MouseButton, bool> MouseStatePrev;
    int MousePositionNowX;
    int MousePositionNowY;
    
    void UpdateMousePos(LPARAM lParam);
};

extern InputState* gINPUTSTATE;

void InitGlobalInput();

LRESULT CALLBACK Input_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


