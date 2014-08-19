#pragma once
#include <Windows.h>
#include <tchar.h>


bool GenerateWindow(WNDPROC WndProc, const char* windowTitle, int width, int height);
void ActivateWindow();	

void UpdateWindowMsgs();