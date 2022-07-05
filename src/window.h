#include <windows.h>
#include <GL/glew.h>
#include <functional>
#include <string>

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

typedef int EventType;
const EventType KEYDOWN_EVENT = 0;
const EventType KEYUP_EVENT = 1;
const EventType MOUSEMOVE_EVENT = 2;
const EventType RESIZE_EVENT = 3;

typedef void (*WinEventCallback)(EventType, unsigned long, unsigned long);

struct ApplicationWindow {
    HWND window{};
    HDC deviceContext{};
    HGLRC renderContext{};
    WinEventCallback eventCallback = nullptr;
    int width = 0, height = 0;
    bool running = true;
};

ApplicationWindow *
WINAPI NewWindow(HINSTANCE hInstance, WinEventCallback eventCallback, const std::string &title, int w, int h);

bool HandleWindowMessage();

void disableMouse(HWND hWnd);

#endif //GUI_WINDOW_H
