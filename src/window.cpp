#include <tchar.h>
#include <iostream>
#include <bits/stdc++.h>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "window.h"

static TCHAR szWindowClass[] = TEXT("myWindowClass");

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

std::unique_ptr<WNDCLASSEX> wcex;

int InitGL(GLvoid)                                      // All Setup For OpenGL Goes Here
{
  if(glewInit() != GLEW_OK) {
    printf("Could not init glew");
    return FALSE;
  };
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
  glViewport(0,0,width,height);                       // Reset The Current Viewport

  glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
  glLoadIdentity();                                   // Reset The Projection Matrix

  // Calculate The Aspect Ratio Of The Window
//  gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);


  glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
  glLoadIdentity();                                   // Reset The Modelview Matrix
}


ApplicationWindow* WINAPI NewWindow(HINSTANCE hInstance, WinEventCallback eventCallback, char *title, int width, int height) {
  if(!wcex) {
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

  if(!RegisterClassEx(wcex.get())) {
    std::cout << "Could not register class" << std::endl;
    exit(1);
  }

  auto hWnd = CreateWindow(szWindowClass, TEXT(title), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           width, height, nullptr, nullptr, hInstance, nullptr);

  if(!hWnd) {
    std::cout << "Could not create window" << std::endl;
    exit(1);
  }
  BYTE bits = 24;
  static  PIXELFORMATDESCRIPTOR pfd=              // pfd Tells Windows How We Want Things To Be
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
  if(!(hDC = GetDC(hWnd))) {
    printf("ERROR: Could not get DC");
    exit(1);
  }
  int PixelFormat;
  if(!(PixelFormat=ChoosePixelFormat(hDC, &pfd))) {
    printf("ERROR: Could not choose pixel format");
    exit(1);
  }

  if(!SetPixelFormat(hDC,PixelFormat,&pfd))       // Are We Able To Set The Pixel Format?
  {
    printf("ERROR: Could not set pixel format");
    exit(1);
  }

  HGLRC hRC = wglCreateContext(hDC);
  if(!hRC) {
    printf("ERROR: Could not create render context");
    exit(1);
  }
  // Setup Dear ImGui binding
//  InitImGui(hWnd);


  if(!wglMakeCurrent(hDC, hRC)) {
    printf("ERROR: Could not activate render context");
    exit(1);
  }

  auto appWindow = static_cast<ApplicationWindow *>(malloc(sizeof(ApplicationWindow)));
  *appWindow = ApplicationWindow{hWnd, hDC, hRC, eventCallback, width, height};
  SetLastError(0);
  int retVal = SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)appWindow);

  ShowWindow(hWnd, SW_SHOW);
  SetForegroundWindow(hWnd);
  SetFocus(hWnd);
  UpdateWindow(hWnd);

  if(!InitGL()) {
    printf("ERROR: Could not init openGL");
    exit(1);
  }
  ReSizeGLScene(width, height);

  return appWindow;
}

bool HandleWindowMessage(HWND hWnd) {
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
  {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
  return msg.message != WM_QUIT;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    return true;
  auto *appWin = reinterpret_cast<ApplicationWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  switch(message) {
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
      appWin->eventCallback(MOUSEMOVE_EVENT, wParam, lParam);
      break;
    case WM_DPICHANGED:
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
      {
        //const int dpi = HIWORD(wParam);
        //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
        const RECT* suggested_rect = (RECT*)lParam;
        ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
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