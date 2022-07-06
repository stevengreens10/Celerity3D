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

glm::vec4 color(int i, int i1, int i2);

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

  Shader *colorShader = Shader::CreateShader("color");
  Shader *lightShader = Shader::CreateShader("light");

  auto texture = std::make_shared<Texture>("assets/images/uvtest.png");

  Material m(*lightShader, nullptr);

  m.matData.ambientColor = {0.857576, 0.857576, 0.857576};
  m.matData.diffuseColor = color(90, 209, 114, 1);
  m.matData.specColor = {0.718182, 0.718182, 0.718182};
  m.matData.shininess = 551.193766;
  m.matData.refractionIndex = 1.45f;
  m.matData.alpha = 1.0f;

  m.ambientTex = texture;

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

  glm::vec3 intensities = glm::vec3(0.4f, 0.5f, 1.0f);
  glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 material = glm::vec3(1.0f, 1.0f, 100);
  while (win->running) {
    HandleWindowMessage();

    Renderer::NewFrame();

    renderer->SetProjection(win->width, win->height);
#ifdef DEBUG
    ImGui::Begin("Debug");
    ImGui::SliderFloat3("lightPos", &(light.pos.x), -300, 300);
    ImGui::SliderFloat3("lightColor", &(lightColor.x), 0.0f, 1.0f);
    ImGui::SliderFloat3("lightIntensity", &(intensities.x), 0, 2);
    ImGui::NewLine();
    ImGui::SliderFloat3("ambientColor", &(m.matData.ambientColor.x), 0.0f, 1.0f);
    ImGui::SliderFloat3("diffuseColor", &(m.matData.diffuseColor.x), 0.0f, 1.0f);
    ImGui::SliderFloat3("specColor", &(m.matData.specColor.x), 0.0f, 1.0f);
    ImGui::SliderFloat("shininess", &(m.matData.shininess), 1.0f, 1000.0f);
    ImGui::SliderFloat("alpha", &(m.matData.alpha), 0.0f, 1.0f);
    ImGui::NewLine();
    ImGui::Text("(%f, %f, %f)", renderer->cameraPos.x, renderer->cameraPos.y, renderer->cameraPos.z);
    ImGui::End();
#endif
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