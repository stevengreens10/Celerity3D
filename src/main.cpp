#include <windows.h>
#include <windowsx.h>
#include <cstdio>
#include "window.h"
#include "util.h"
#include "graphics/VertexArray.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_win32.h"
#include "GLFW/glfw3.h"
#include "graphics/Cube.h"
#include <glm/glm.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

Shader *shader;
Renderer *renderer;
ApplicationWindow *win;

float mouseX, mouseY;

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
    if(win) {
      glViewport(0,0,win->width,win->height);
      renderer->SetProjection(win->width, win->height);
    }
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {


  win = NewWindow(hInstance, &handleEvent, "Test window", INIT_WIDTH, INIT_HEIGHT);
  renderer = new Renderer();
  renderer->Init(win);

  auto texture = std::make_shared<Texture>("assets/images/square.png");
  Cube cube(texture.get(), renderer->MAIN_SHADER.get());

  bool debug = true;
  while(win->running) {
    HandleWindowMessage(win->window);

    renderer->NewFrame();


    if(debug){
      ImGui::Begin("Settings");
      ImGui::SliderFloat("modelScale", &cube.scale, 50, 1000);

      ImGui::SliderFloat("modelX", &cube.pos.x, 0, win->width);
      ImGui::SliderFloat("modelY", &cube.pos.y, 0, win->height);
      ImGui::SliderFloat("modelZ", &cube.pos.z, 0.2, 5);

      ImGui::SliderFloat3("modelRotate", &cube.rot.x, 0, 359);

      ImGui::Text("%.3f ms/frame (%.1f) FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    renderer->Draw(cube);

    renderer->Update(win);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
  ImGui_ImplWin32_Shutdown();
  wglDeleteContext(win->renderContext);

  return 0;
}
#pragma clang diagnostic pop
