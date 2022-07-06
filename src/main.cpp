#include <windows.h>
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
#include "graphics/Mesh.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

Shader *shader;
Renderer *renderer;
ApplicationWindow *win;

using std::cos, std::sin, std::acos;

void handleEvent(EventType type, unsigned long p1, unsigned long p2) {
  if (type == KEYDOWN_EVENT) {
    if (p1 == VK_ESCAPE) {
      // Lose focus on escape
      SetFocus(nullptr);
    }

    float speed = 5.0f;

    glm::vec3 horizontalDir = glm::cross(glm::vec3(0, 1, 0), renderer->cameraDir) * speed;
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

    renderer->CalculateView();

  } else if (type == MOUSEMOVE_EVENT) {
    auto dx = (int) p1;
    auto dy = (int) -p2;
    float sens = 0.1f;

    float yawVal = (float) dx * sens;
    float pitchVal = (float) dy * sens;
    if (pitchVal > 89.0f)
      pitchVal = 89.0f;
    if (pitchVal < -89.0f)
      pitchVal = -89.0f;
    renderer->RotateCamera(renderer->yaw + yawVal, renderer->pitch + pitchVal);

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
  printf("Version: %s\n", glGetString(GL_VERSION));
  printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  renderer = new Renderer();
  renderer->Init(win);

  Shader *mainShader = Shader::CreateShader("shader");
  Shader *colorShader = Shader::CreateShader("color");

  auto texture = std::make_shared<Texture>("assets/images/square.png");
  Material mat1(*mainShader, texture.get());

  Material mat2(*colorShader, nullptr);
  float color[] = {0.1f, 0.1f, 0.1f};
  mat2.SetUniform("u_color", U3f, color);

  Cube cube(mat1);
  cube.pos = glm::vec3(0, 0, 0.0f);
  cube.scale = 75;

  auto meshTex = std::make_shared<Texture>("assets/images/meshTexture.png");
  Mesh mesh("assets/mesh/gun.obj", mat2);
  mesh.pos = glm::vec3(220, -10, 30);
  mesh.scale = 50;

  bool debug = true;
  while (win->running) {
    HandleWindowMessage();

    Renderer::NewFrame();

    renderer->SetProjection(win->width, win->height);
    if (debug) {
      ImGui::ShowDemoWindow(&debug);
      renderer->DrawRenderableDebug("Mesh", &mesh);
    }

    renderer->Draw(cube);
    renderer->Draw(mesh);

    Renderer::Update(win);
  }

  // Cleanup
  Renderer::Cleanup(win);

  return 0;
}