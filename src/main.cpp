#include <windows.h>
#include <windowsx.h>
#include <cstdio>
#include "window.h"
#include "GL/wglew.h"
#include "log.h"
#include "util.h"
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_win32.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <advpub.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

Shader *shader;
Renderer *renderer;
ApplicationWindow *win;

float mouseX, mouseY;

void InitImGui(HWND hWnd) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

  ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplWin32_InitForOpenGL(hWnd, win->renderContext);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void UpdateImGui() {
  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    HDC backup_current_context = GetDC(win->window);
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    wglMakeCurrent(backup_current_context, win->renderContext);
  }
}

void handleEvent(EventType type, unsigned long p1, unsigned long p2) {
  if(type == KEYDOWN_EVENT) {
    printf("Key pressed: %x\n", p1);
    if(p1 == VK_ESCAPE) {
      printf("Closing\n");
      win->running = false;
    }
  } else if(type == MOUSEMOVE_EVENT) {
    mouseX = (float) GET_X_LPARAM(p2);
    mouseY = (float) win->height - GET_Y_LPARAM(p2);
  } else if(type == RESIZE_EVENT) {
    if(shader) {
      glViewport(0,0,win->width,win->height);
      shader->Bind();
      shader->SetUniformMat4f("u_MVP", renderer->GetMVP(win->width, win->height));
    }
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {

  ImGui_ImplWin32_EnableDpiAwareness();
  win = NewWindow(hInstance, &handleEvent, "Test window", INIT_WIDTH, INIT_HEIGHT);

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&GLDebugMessageCallback, nullptr);

  wglSwapIntervalEXT(1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  InitImGui(win->window);

  float size = 100;
  float x = 100, y = 100, z = 0;
  Vertex vertices[] = {
          // Front vertices
          {{x, y, z},{0.0f, 0.0f}},
          {{x+size, y, z},{1.0f, 0.0f}},
          {{x+size, y+size, z},{1.0f, 1.0f}},
          {{x, y+size, z},{0.0f, 1.0f}},
          // Back vertices
          {{x, y, z-size},{0.0f, 0.0f}},
          {{x+size, y, z-size},{1.0f, 0.0f}},
          {{x+size, y+size, z-size},{1.0f, 1.0f}},
          {{x, y+size, z-size},{0.0f, 1.0f}}
  };

  uint32_t indices[] = {
          // Front face
          0, 1, 2,
          2, 3, 0,
          // Right Face
          1, 5, 2,
          2, 6, 5,
          // Left Face
          0, 4, 3,
          3, 7, 4,
          // Back Face
          4, 5, 6,
          6, 7, 4,
          // Top Face
          3, 2, 6,
          6, 7, 3,
          // Bottom Face
          0, 1, 5,
          5, 4, 0,

  };

  VertexArray vertArr;
  VertexBuffer vertBuf(vertices, sizeof(vertices));

  BufferLayout layout = BufferLayout();
  layout.Push<float>(3);
  layout.Push<float>(2);
  vertArr.AddBuffer(vertBuf, layout);

  IndexBuffer idxBuf(indices, sizeof(indices) / sizeof(uint32_t));

  shader = new Shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");
  Texture texture("assets/images/corgi.png");

  texture.Bind();
  shader->Bind();
  shader->SetUniform1i("u_Texture", 0);

  renderer = new Renderer();
  shader->SetUniformMat4f("u_MVP", renderer->GetMVP(win->width, win->height));

  while(win->running) {
    HandleWindowMessage(win->window);

    renderer->Clear();
    renderer->Draw(*shader, vertArr, idxBuf);

    renderer->DrawImGui();
    UpdateImGui();
    SwapBuffers(win->deviceContext);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
  ImGui_ImplWin32_Shutdown();
  wglDeleteContext(win->renderContext);

  return 0;
}
#pragma clang diagnostic pop
