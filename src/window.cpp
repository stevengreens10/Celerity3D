#include <tchar.h>
#include <iostream>
#include <bits/stdc++.h>

#include <imgui.h>
#include <imgui_internal.h>

#include "window.h"

static TCHAR szWindowClass[] = TEXT("myWindowClass");

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

std::unique_ptr<WNDCLASSEX> wcex;

int InitGL(GLvoid)                                      // All Setup For OpenGL Goes Here
{
  if (glewInit() != GLEW_OK) {
    printf("Could not init glew");
    return FALSE;
  }
  glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
  glClearColor(0.20f, 0.90f, 0.92f, 0.5f);               // Set Background
  glClearDepth(1.0f);                                 // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
  return TRUE;                                        // Initialization Went OK
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)     // Resize And Initialize The GL Window
{
  glViewport(0, 0, width, height);                       // Reset The Current Viewport

  glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
  glLoadIdentity();                                   // Reset The Projection Matrix

  // Calculate The Aspect Ratio Of The Window
//  gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);


  glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
  glLoadIdentity();                                   // Reset The Modelview Matrix
}


ApplicationWindow *
WINAPI NewWindow(HINSTANCE hInstance, WinEventCallback eventCallback, const std::string &title, int width, int height) {
  if (!wcex) {
    wcex = std::make_unique<WNDCLASSEX>();
    wcex->cbSize = sizeof(WNDCLASSEX);
    wcex->style = CS_HREDRAW | CS_VREDRAW;
    wcex->lpfnWndProc = WndProc;
    wcex->cbClsExtra = 0;
    wcex->cbWndExtra = 0;
    wcex->hInstance = hInstance;
    wcex->hIcon = LoadIcon(hInstance, IDI_EXCLAMATION);
    wcex->hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex->hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex->lpszMenuName = nullptr;
    wcex->lpszClassName = szWindowClass;
    wcex->hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
  }

  if (!RegisterClassEx(wcex.get())) {
    std::cout << "Could not register class" << std::endl;
    exit(1);
  }

  auto hWnd = CreateWindow(szWindowClass, TEXT(title.c_str()), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           width, height, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd) {
    std::cout << "Could not create window" << std::endl;
    exit(1);
  }
  BYTE bits = 24;
  static PIXELFORMATDESCRIPTOR pfd =              // pfd Tells Windows How We Want Things To Be
          {
                  sizeof(PIXELFORMATDESCRIPTOR),              // Size Of This Pixel Format Descriptor
                  1,                                          // Version Number
                  PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
                  PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
                  PFD_DOUBLEBUFFER,                           // Must Support Double Buffering
                  PFD_TYPE_RGBA,                              // Request An RGBA Format
                  bits,                                       // Select Our Color Depth
                  0, 0, 0, 0, 0, 0,                           // Color Bits Ignored
                  0,                                          // No Alpha Buffer
                  0,                                          // Shift Bit Ignored
                  0,                                          // No Accumulation Buffer
                  0, 0, 0, 0,                                 // Accumulation Bits Ignored
                  16,                                         // 16Bit Z-Buffer (Depth Buffer)
                  0,                                          // No Stencil Buffer
                  0,                                          // No Auxiliary Buffer
                  PFD_MAIN_PLANE,                             // Main Drawing Layer
                  0,                                          // Reserved
                  0, 0, 0                                     // Layer Masks Ignored
          };

  HDC hDC;
  if (!(hDC = GetDC(hWnd))) {
    printf("ERROR: Could not get DC");
    exit(1);
  }
  int PixelFormat;
  if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
    printf("ERROR: Could not choose pixel format");
    exit(1);
  }

  if (!SetPixelFormat(hDC, PixelFormat, &pfd))       // Are We Able To Set The Pixel Format?
  {
    printf("ERROR: Could not set pixel format");
    exit(1);
  }

  HGLRC hRC = wglCreateContext(hDC);
  if (!hRC) {
    printf("ERROR: Could not create render context");
    exit(1);
  }

  if (!wglMakeCurrent(hDC, hRC)) {
    printf("ERROR: Could not activate render context");
    exit(1);
  }

  auto appWindow = static_cast<ApplicationWindow *>(malloc(sizeof(ApplicationWindow)));
  *appWindow = ApplicationWindow{hWnd, hDC, hRC, eventCallback, width, height};
  SetLastError(0);
  SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) appWindow);

  ShowWindow(hWnd, SW_SHOW);
  SetForegroundWindow(hWnd);
  SetFocus(hWnd);
  UpdateWindow(hWnd);

  if (!InitGL()) {
    printf("ERROR: Could not init openGL");
    exit(1);
  }
  ReSizeGLScene(width, height);

  return appWindow;
}

bool HandleWindowMessage() {
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
  return msg.message != WM_QUIT;
}

bool mouseDisabled = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    return true;
  auto *appWin = reinterpret_cast<ApplicationWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  switch (message) {
    case WM_SIZE:
      appWin->width = LOWORD(lParam);
      appWin->height = HIWORD(lParam);
      appWin->eventCallback(RESIZE_EVENT, wParam, lParam);
      break;
    case WM_KEYUP:
      appWin->eventCallback(KEYUP_EVENT, wParam, lParam);
      break;
    case WM_KEYDOWN:
      appWin->eventCallback(KEYDOWN_EVENT, wParam, lParam);
      break;
    case WM_MOUSEMOVE:
//      appWin->eventCallback(MOUSEMOVE_EVENT, wParam, lParam);
      break;
    case WM_KILLFOCUS:
      if (mouseDisabled)
        enableMouse();
      break;
    case WM_LBUTTONDOWN:
    case WM_SETFOCUS: {
      bool imGuiWantsMouse = ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
      if (!mouseDisabled && !imGuiWantsMouse) {
        disableMouse(hWnd);
        SetFocus(hWnd);
      }
      break;
    }
    case WM_INPUT: {
      UINT size = 0;
      auto ri = (HRAWINPUT) lParam;
      RAWINPUT *data;
      int dx, dy;
      GetRawInputData(ri, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
      data = static_cast<RAWINPUT *>(calloc(1, sizeof(RAWINPUT)));

      if (GetRawInputData(ri, RID_INPUT,
                          data, &size,
                          sizeof(RAWINPUTHEADER)) == (UINT) -1) {
        break;
      }

      if (data->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
        printf("Absolute mouse mode not supported yet\n");
        dx = data->data.mouse.lLastX;// - lastCursorPosX;
        dy = data->data.mouse.lLastY;// - lastCursorPosY;
      } else {
        dx = data->data.mouse.lLastX;
        dy = data->data.mouse.lLastY;
      }

      appWin->eventCallback(MOUSEMOVE_EVENT, dx, dy);

      break;
    }
    case WM_DPICHANGED:
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports) {
        //const int dpi = HIWORD(wParam);
        //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
        const RECT *suggested_rect = (RECT *) lParam;
        ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top,
                       suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top,
                       SWP_NOZORDER | SWP_NOACTIVATE);
      }
      break;
    case WM_CLOSE:
    case WM_DESTROY:
      appWin->running = false;
      PostQuitMessage(0);
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}


void disableMouse(HWND hWnd) {
  mouseDisabled = true;
  const RAWINPUTDEVICE rid = {0x01, 0x02, 0, hWnd};
  if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
    printf("Error: Could not register raw mouse input");
  }

  RECT clipRect;
  GetClientRect(hWnd, &clipRect);
  ClientToScreen(hWnd, (POINT *) &clipRect.left);
  ClientToScreen(hWnd, (POINT *) &clipRect.right);
  ClipCursor(&clipRect);
  ShowCursor(false);
}

void enableMouse() {
  mouseDisabled = false;
  const RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
  ClipCursor(nullptr);
  ShowCursor(true);
  if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
    printf("Error: %s\n",
           "Win32: Failed to remove raw input device");
  }
}