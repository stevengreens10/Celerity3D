#include <windows.h>
#include <cstdio>
#include <sstream>
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
#include "graphics/Scene.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

glm::vec4 color(int i, int i1, int i2);

using std::cos, std::sin, std::acos;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  {
    ImGui_ImplWin32_EnableDpiAwareness();
    Application::window = NewWindow(hInstance, "Test window", INIT_WIDTH, INIT_HEIGHT);
    Application::Init();
//    Log::file("log.txt");
    Log::logf("Version: %s", glGetString(GL_VERSION));
    Log::logf("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    Shader *colorShader = Shader::CreateShader("color");
    Shader *lightShader = Shader::CreateShader("light");

    Material m(*lightShader, "testMaterial");

    Scene scene;

    Mesh cube("assets/mesh/cube.obj", m);
    cube.pos = glm::vec3(0, 0, 0.0f);
    cube.scale = 50;
    scene.AddObject(&cube);

//    Mesh mesh("assets/mesh/backpack.obj", m);
//    mesh.pos = glm::vec3(220, -10, 30);
//    mesh.scale = 50;
//    scene.AddObject(&mesh);

    LightSource light{};
    light.type = LIGHT_POINT;
    light.pos = glm::vec3(110.0f, 0.0f, -300.0f);
    light.intensities = glm::vec3(0.05f, 1.0f, 1.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    scene.AddLight(&light);

    Material lightSource(*colorShader, "lightMat");
    Cube lightCube(lightSource);
    lightCube.pos = light.pos;
    lightCube.scale = 25;
    scene.AddObject(&lightCube);

    const Renderer &renderer = *Application::renderer;
    while (true) {
      HandleWindowMessage();
      Application::Update();
      if (!Application::running) {
        break;
      }

      Renderer::NewFrame();

#ifdef IMGUI
      ImGui::ShowDemoWindow();
      ImGui::Begin("Debug");
      if (ImGui::TreeNode("Lights")) {
        int idx = 0;
        for (auto sceneLight: scene.Lights()) {
          std::stringstream name;
          name << idx++;
          if (ImGui::TreeNode(name.str().c_str())) {
            ImGui::SliderFloat3("Position", &(sceneLight->pos.x), -300, 300);
            ImGui::SliderFloat3("Color", &(sceneLight->color.x), 0.0f, 1.0f);
            ImGui::SliderFloat3("Intensity", &(sceneLight->intensities.x), 0, 2);
            ImGui::TreePop();
          }
        }
        ImGui::TreePop();
      }
      ImGui::NewLine();
      if (ImGui::TreeNode("Meshes")) {
        int idx = 0;
        for (auto sceneObj: scene.Objects()) {
          std::stringstream name;
          name << idx++;
          if (ImGui::TreeNode(name.str().c_str())) {
            ImGui::SliderFloat("Scale", &(sceneObj->scale), 25, 500);
            ImGui::SliderFloat3("Position", &(sceneObj->pos.x), -500, 500);
            ImGui::SliderFloat3("Rotation", &(sceneObj->rot.x), 0, 359);
            ImGui::NewLine();
            if (typeid(*sceneObj) == typeid(Mesh)) {
              auto sceneMesh = (Mesh *) sceneObj;
              for (auto &submesh: sceneMesh->meshes) {
                ImGui::Checkbox(submesh.name.c_str(), &submesh.shouldRender);
              }
            }
            ImGui::TreePop();
          }
        }
        ImGui::TreePop();
      }
      ImGui::NewLine();
      ImGui::Text("(%f, %f, %f)", Camera::Pos().x, Camera::Pos().y, Camera::Pos().z);
      ImGui::Text("%.3f ms/frame (%.1f) FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
#endif
      glm::vec4 lightCol4 = glm::vec4(light.color, 1.0f);
      lightSource.SetUniform("u_color", U4f, &(lightCol4.x));
      lightCube.pos = light.pos;

      renderer.Draw(scene);

      Renderer::EndFrame(Application::window);
    }

    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}