//#include <Windows.h>
//#include <stdio.h>
//#include "imgui.h"
//#include "backends/imgui_impl_win32.h"
//#include "backends/imgui_impl_opengl3.h"
//#include "GL/glew.h"
//#include <tchar.h>
//#include "window.h"
//
//#define IMGUI_IMPL_OPENGL_LOADER_GLAD
//
//LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//
//void handleEvent2(EventType type, unsigned long p1, unsigned long p2) {
//
//}
//
//// Main code
//int main(int, char**)
//{
//  ImGui_ImplWin32_EnableDpiAwareness();
//
////  // Create application window
////  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
////  ::RegisterClassEx(&wc);
////  HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui OpenGl Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, NULL, NULL, wc.hInstance, NULL);
////
////  // Show the window
////  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
////  ::UpdateWindow(hwnd);
////
////  //Initialize OpenGL and Device Context
////  HDC devContext = GetDC(hwnd);
////
////  PIXELFORMATDESCRIPTOR pfd =
////          {
////                  sizeof(PIXELFORMATDESCRIPTOR),
////                  1,
////                  PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
////                  PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
////                  32,                   // Colordepth of the framebuffer.
////                  0, 0, 0, 0, 0, 0,
////                  0,
////                  0,
////                  0,
////                  0, 0, 0, 0,
////                  24,                   // Number of bits for the depthbuffer
////                  8,                    // Number of bits for the stencilbuffer
////                  0,                    // Number of Aux buffers in the framebuffer.
////                  PFD_MAIN_PLANE,
////                  0,
////                  0, 0, 0
////          };
////
////  int pixelFormat = ChoosePixelFormat(devContext, &pfd);
////  SetPixelFormat(devContext, pixelFormat, &pfd);
////  HGLRC glContext = wglCreateContext(devContext);
////
////  wglMakeCurrent(devContext, glContext);
//
//  auto win = NewWindow(GetModuleHandle(NULL), &handleEvent2, "Test", 640, 480);
//  auto hwnd = win->window;
//  auto glContext = win->renderContext;
//  auto devContext = win->deviceContext;
//
//  // Setup Dear ImGui context
//  IMGUI_CHECKVERSION();
//  ImGui::CreateContext();
//  ImGuiIO& io = ImGui::GetIO(); (void)io;
//  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
//  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
//
//  // Setup Dear ImGui style
//  ImGui::StyleColorsDark();
//  //ImGui::StyleColorsClassic();
//
//  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//  ImGuiStyle& style = ImGui::GetStyle();
//  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//  {
//    style.WindowRounding = 0.0f;
//    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//  }
//
////  //Load OpenGL
////  if(glewInit() != GLEW_OK) {
////    printf("Could not init glew");
////    return FALSE;
////  }
//
//  // Setup Platform/Renderer bindings
//  ImGui_ImplWin32_InitForOpenGL(hwnd, glContext);
//  ImGui_ImplOpenGL3_Init("#version 410");
//
//  // Our state
//  bool show_demo_window = true;
//  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
//  // Main loop
//  MSG msg;
//  ZeroMemory(&msg, sizeof(msg));
//  while (true)
//  {
//    HandleWindowMessage(hwnd);
//
//    // Start the Dear ImGui frame
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplWin32_NewFrame();
//    ImGui::NewFrame();
//
//    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//    if (show_demo_window)
//      ImGui::ShowDemoWindow(&show_demo_window);
//
//    // Rendering
//    ImGui::Render();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
//    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//    glClear(GL_COLOR_BUFFER_BIT);
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//    // Update and Render additional Platform Windows
//    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
//    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//    {
//      HDC backup_current_context = GetDC(hwnd);
//      ImGui::UpdatePlatformWindows();
//      ImGui::RenderPlatformWindowsDefault();
//      wglMakeCurrent(backup_current_context, glContext);
//    }
//
//    SwapBuffers(devContext);
//  }
//
//  ImGui_ImplOpenGL3_Shutdown();
//  ImGui_ImplWin32_Shutdown();
//  ImGui::DestroyContext();
//
//  ::DestroyWindow(hwnd);
//
//  return 0;
//}
//
//#ifndef WM_DPICHANGED
//#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
//#endif
//
//
//// Win32 message handler
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//    return true;
//
//  switch (msg)
//  {
//    case WM_SYSCOMMAND:
//      if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
//        return 0;
//      break;
//    case WM_DESTROY:
//      ::PostQuitMessage(0);
//      return 0;
//    case WM_DPICHANGED:
//      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
//      {
//        //const int dpi = HIWORD(wParam);
//        //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
//        const RECT* suggested_rect = (RECT*)lParam;
//        ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
//      }
//      break;
//  }
//  return ::DefWindowProc(hWnd, msg, wParam, lParam);
//}
