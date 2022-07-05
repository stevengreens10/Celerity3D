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
#include "graphics/Cube.h"
#include "backends/imgui_impl_win32.h"
#include "graphics/Material.h"

#include <chrono>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

Shader *shader;
Renderer *renderer;
ApplicationWindow *win;

void handleEvent(EventType type, unsigned long p1, unsigned long p2) {
  if (type == KEYDOWN_EVENT) {
    printf("Key pressed: %x\n", p1);
    if (p1 == VK_ESCAPE) {
      printf("Closing\n");
      win->running = false;
    }

    float speed = 5.0f;
    float sens = 3.0f;
    float angle = acos(renderer->cameraDir.x);

    glm::vec3 horizontalDir = glm::cross(glm::vec3(0, 1, 0), renderer->cameraDir) * speed;
    printf("Angle: %f\n", glm::degrees(angle));
    if (p1 == 'A') {
      renderer->cameraPos += horizontalDir;
    } else if (p1 == 'D') {
      renderer->cameraPos -= horizontalDir;
    }
    if (p1 == VK_SPACE) {
      renderer->cameraPos.y += 1 * speed;
    }
    if (p1 == VK_SHIFT) {
      renderer->cameraPos.y -= 1 * speed;
    }
    if (p1 == 'W') {
      renderer->cameraPos += glm::vec3(renderer->cameraDir.x, 0, renderer->cameraDir.z) * speed;
    } else if (p1 == 'S') {
      renderer->cameraPos -= glm::vec3(renderer->cameraDir.x, 0, renderer->cameraDir.z) * speed;
    }
    if (p1 == 'Q') {
      angle -= glm::radians(1.0f * sens);
      renderer->cameraDir.x = cos(angle);
      renderer->cameraDir.z = sin(angle);
    } else if (p1 == 'E') {
      angle += glm::radians(1.0f * sens);
      renderer->cameraDir.x = cos(angle);
      renderer->cameraDir.z = sin(angle);
    }


  } else if (type == MOUSEMOVE_EVENT) {
    float mouseX = (float) GET_X_LPARAM(p2);
    float mouseY = (float) win->height - GET_Y_LPARAM(p2);

    float centerX = ((float) win->width) / 2;
    float centerY = ((float) win->height) / 2;
    float xDiff = mouseX - centerX;
    float yDiff = mouseY - centerY - 34;
    float sens = 0.1f;
    printf("(%f, %f)\n", xDiff, yDiff);

    float yawVal = xDiff * sens;
    float pitchVal = yDiff * sens;
    renderer->RotateCamera(renderer->yaw + yawVal, renderer->pitch + pitchVal);

    SetCursorPos(centerX, centerY);

  } else if (type == RESIZE_EVENT) {
    if (win) {
      glViewport(0, 0, win->width, win->height);
      renderer->SetProjection(win->width, win->height);
    }
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {

  ImGui_ImplWin32_EnableDpiAwareness();
  win = NewWindow(hInstance, &handleEvent, "Test window", INIT_WIDTH, INIT_HEIGHT);
  ShowCursor(false);
  renderer = new Renderer();
  renderer->Init(win);

  auto texture = std::make_shared<Texture>("assets/images/square.png");
  auto texture2 = std::make_shared<Texture>("assets/images/corgi.png");
  Material cubeMat(*Renderer::MAIN_SHADER, *texture);
  Material cube2Mat(*Renderer::MAIN_SHADER, *texture2);
  Cube cube(cubeMat);
  Cube cube2(cube2Mat);
  cube.pos = glm::vec3(0, 0, 0.0f);
  cube.scale = 75;

  cube2.pos = glm::vec3(50, -10, 30);
  cube2.scale = 50;

  bool debug = true;
  auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  while (win->running) {
    HandleWindowMessage(win->window);

    renderer->NewFrame();

    renderer->SetProjection(win->width, win->height);
    if (debug) {
      ImGui::ShowDemoWindow(&debug);
      renderer->DrawRenderableDebug("Cube1", &cube, win);
    }

    renderer->Draw(cube);
    renderer->Draw(cube2);

    renderer->Update(win);
  }

  // Cleanup
  renderer->Cleanup(win);

  return 0;
}

#pragma clang diagnostic pop
