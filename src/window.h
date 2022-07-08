#include <windows.h>
#include <GL/glew.h>
#include <functional>
#include <string>
//#include "EventHandler.h"

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

struct Window {
    HWND hWnd{};
    HDC deviceContext{};
    HGLRC renderContext{};
    int width = 0, height = 0;
};

Window *
WINAPI NewWindow(HINSTANCE hInstance, const std::string &title, int w, int h);

bool HandleWindowMessage();

void enableMouse();

void disableMouse(HWND hWnd);

#endif //GUI_WINDOW_H
