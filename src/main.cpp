#include <windows.h>
#include <cstdio>
#include "window.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "backends/imgui_impl_opengl3.h"
#include "graphics/Cube.h"
#include "backends/imgui_impl_win32.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "log.h"
#include "Camera.h"
#include "Application.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

glm::vec4 color(int i, int i1, int i2);

using std::cos, std::sin, std::acos;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  {
    ImGui_ImplWin32_EnableDpiAwareness();
    Application::window = NewWindow(hInstance, "Test window", INIT_WIDTH, INIT_HEIGHT);
//    Log::file("log.txt");
    Log::logf("Version: %s", glGetString(GL_VERSION));
    Log::logf("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    Application::Init();

    Shader *colorShader = Shader::CreateShader("color");
    Shader *lightShader = Shader::CreateShader("light");

    Material m(*lightShader, "testMaterial");

    Mesh cube("assets/mesh/cube.obj", m);
    cube.pos = glm::vec3(0, 0, 0.0f);
    cube.scale = 50;

    Mesh mesh("assets/mesh/backpack.obj", m);
    mesh.pos = glm::vec3(220, -10, 30);
    mesh.scale = 50;

    Material lightSource(*colorShader, "lightMat");
    Cube light(lightSource);
    light.pos = glm::vec3(110.0f, 0.0f, -300.0f);
    light.scale = 25;

    glm::vec3 intensities = glm::vec3(0.05f, 1.0f, 1.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    struct __attribute__ ((packed)) Scene {
        glm::vec3 camPos;
        glm::vec3 lightPos;
        glm::vec3 lightColor;
        glm::vec3 lightIntensities;
    };

    Scene s{};

    bool autoRotate = false;
    const Renderer &renderer = *Application::renderer;
    while (true) {
      HandleWindowMessage();
      Application::Update();
      if (!Application::running) {
        break;
      }

      Renderer::NewFrame();

#ifdef IMGUI
      ImGui::Begin("Debug");
      ImGui::SliderFloat3("lightPos", &(light.pos.x), -300, 300);
      ImGui::SliderFloat3("lightColor", &(lightColor.x), 0.0f, 1.0f);
      ImGui::SliderFloat3("lightIntensity", &(intensities.x), 0, 2);
      ImGui::NewLine();
      ImGui::SliderFloat("meshScale", &(mesh.scale), 25, 500);
      ImGui::SliderFloat3("meshRotate", &(mesh.rot.x), 0, 359);
      ImGui::Checkbox("AutoRotate", &autoRotate);
      ImGui::NewLine();
      for (auto &submesh: mesh.meshes) {
        ImGui::Checkbox(submesh.name.c_str(), &submesh.shouldRender);
      }
      ImGui::NewLine();
      ImGui::Text("(%f, %f, %f)", Camera::Pos().x, Camera::Pos().y, Camera::Pos().z);
      ImGui::Text("%.3f ms/frame (%.1f) FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
#endif
      glm::vec4 lightCol4 = glm::vec4(lightColor, 1.0f);
      lightSource.SetUniform("u_color", U4f, &(lightCol4.x));
      s.camPos = Camera::Pos();
      s.lightPos = light.pos;
      s.lightColor = lightColor;
      s.lightIntensities = intensities;
      Shader::SetGlobalUniform("Scene", (char *) &s);
      renderer.Draw(light);
      renderer.Draw(cube);
      renderer.Draw(mesh);

      if (autoRotate)
        mesh.rot.y += 1;
      if (mesh.rot.y > 360)
        mesh.rot.y = mesh.rot.y - 360;

      Renderer::EndFrame(Application::window);
    }

    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}