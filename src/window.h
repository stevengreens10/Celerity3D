#include <windows.h>
#include <GL/glew.h>
#include <functional>

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

typedef int EventType;
const EventType KEYDOWN_EVENT = 0;
const EventType KEYUP_EVENT = 1;
const EventType MOUSEMOVE_EVENT = 2;
const EventType RESIZE_EVENT = 3;

//typedef std::function<void(EventType, unsigned long, unsigned long)> WinEventCallback;
typedef void (*WinEventCallback)(EventType, unsigned long, unsigned long);

typedef struct {
  HWND window;
  HDC deviceContext;
  HGLRC renderContext;
  WinEventCallback eventCallback;
  int width, height;
  bool running = true;
} ApplicationWindow;

ApplicationWindow * WINAPI NewWindow(HINSTANCE hInstance, WinEventCallback eventCallback, char *title, int w, int h);
bool HandleWindowMessage(HWND hWnd);

#endif //GUI_WINDOW_H
