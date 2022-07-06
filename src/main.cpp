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
  Shader *lightShader = Shader::CreateShader("light");

//  auto texture = std::make_shared<Texture>("assets/images/square.png");

  Material m(*lightShader, nullptr);
  glm::vec4 col = color(66, 135, 245, 255);
  m.SetUniform("u_color", U4f, &(col.x));

  Mesh cube("assets/mesh/cube.obj", m);
  cube.pos = glm::vec3(0, 0, 0.0f);
  cube.scale = 50;

  Mesh mesh("assets/mesh/gun.obj", m);
  mesh.pos = glm::vec3(220, -10, 30);
  mesh.scale = 50;

  Material lightSource(*colorShader, nullptr);
  Cube light(lightSource);
  light.pos = glm::vec3(110.0f, 0.0f, -300.0f);
  light.scale = 25;

  bool debug = true;
  glm::vec3 intensities = glm::vec3(0.4f, 0.5f, 1.0f);
  glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 material = glm::vec3(1.0f, 1.0f, 100);
  while (win->running) {
    HandleWindowMessage();

    Renderer::NewFrame();

    renderer->SetProjection(win->width, win->height);
    if (debug) {
      ImGui::Begin("Light");
      ImGui::SliderFloat3("lightPos", &(light.pos.x), -300, 300);
      ImGui::SliderFloat3("lightColor", &(lightColor.x), 0.0f, 1.0f);
      ImGui::SliderFloat3("lightIntensity", &(intensities.x), 0, 2);
      ImGui::SliderFloat("ambient strength", &(material.x), 0, 2);
      ImGui::SliderFloat("diffuse strength", &(material.y), 0, 2);
      ImGui::SliderFloat("shininess", &(material.z), 0, 1000);
      ImGui::Text("(%f, %f, %f)", renderer->cameraPos.x, renderer->cameraPos.y, renderer->cameraPos.z);
      ImGui::End();
    }
    glm::vec4 lightCol4 = glm::vec4(lightColor, 1.0f);
    lightSource.SetUniform("u_color", U4f, &(lightCol4.x));
    m.SetUniform("u_cameraPos", U3f, &(renderer->cameraPos.x));
    m.SetUniform("u_lightPos", U3f, &(light.pos.x));
    m.SetUniform("u_lightColor", U3f, &(lightColor.x));
    m.SetUniform("u_lightIntensities", U3f, &(intensities.x));
    m.SetUniform("u_material", U3f, &(material.x));

    renderer->Draw(light);
    renderer->Draw(cube);
    renderer->Draw(mesh);

    Renderer::Update(win);
  }

  // Cleanup
  Renderer::Cleanup(win);

  return 0;
}