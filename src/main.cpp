#include <windows.h>
#include <cstdio>
#include <sstream>
#include "window.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "backends/imgui_impl_opengl3.h"
#include "graphics/object/primitive/Cube.h"
#include "backends/imgui_impl_win32.h"
#include "graphics/Material.h"
#include "graphics/object/Mesh.h"
#include "log.h"
#include "Camera.h"
#include "Input.h"
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
    Log::file("log.txt");
    Log::logf("Version: %s", glGetString(GL_VERSION));
    Log::logf("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    Shader *colorShader = Shader::CreateShader("color");
    Shader::CreateShader("light");

    Scene scene;

    Mesh cube("assets/mesh/cube.obj");
    cube.pos = glm::vec3(0, 0, 0.0f);
    cube.scale = 1;
    scene.AddObject(&cube);

//    Mesh mesh("assets/mesh/backpack.obj");
//    mesh.pos = glm::vec3(0, 0, 0);
//    mesh.scale = 1;
//    scene.AddObject(&mesh);

    unordered_map<LightSource *, Object *> lightToObj;
    LightSource light{};
    light.type = LIGHT_POINT;
    light.pos = glm::vec3(0.0f, 0.0f, -5.0f);
    light.intensities = glm::vec3(0.01f, 0.8f, 1.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    scene.AddLight(&light);

    Material lightSource(*colorShader, "lightMat");
    Cube lightCube(lightSource);
    lightCube.pos = light.pos;
    lightCube.scale = 0.3;
    scene.AddObject(&lightCube);
    lightToObj[&light] = &lightCube;

    const Renderer &renderer = *Application::renderer;

    float angle = 0;
    float radius = 7.5;
    int framesSinceAdd = 30;

    while (true) {
      framesSinceAdd++;
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
            ImGui::SliderFloat3("Position", &(sceneLight->pos.x), -20, 20);
            ImGui::SliderFloat3("Color", &(sceneLight->color.x), 0.0f, 1.0f);
            ImGui::SliderFloat3("Intensity", &(sceneLight->intensities.x), 0, 2);
            ImGui::SliderFloat("attenConst", &(sceneLight->attenuation.x), 0, 2);
            ImGui::SliderFloat("attenLinear", &(sceneLight->attenuation.y), 0, 0.2);
            ImGui::SliderFloat("attenQuadratic", &(sceneLight->attenuation.z), 0, 0.05);
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
            ImGui::SliderFloat("Scale", &(sceneObj->scale), 0, 10);
            ImGui::SliderFloat3("Position", &(sceneObj->pos.x), -20, 20);
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
      // Make lights move around and map cube pos and color to light source
      angle += 0.75f;
      for (auto l: scene.Lights()) {
        l->pos.x += radius * std::cos(glm::radians(angle)) * 0.01;
        l->pos.z += radius * std::sin(glm::radians(angle)) * 0.01;
        auto o = (Primitive *) lightToObj[l];
        if (o) {
          o->pos = l->pos;
          o->material.get().matData.diffuseColor = l->color;
        }
      }

      // Add lights dynamically
      if (Input::IsPressed(VK_TAB) && framesSinceAdd >= 30) {
        framesSinceAdd = 0;
        auto l = new LightSource();
        l->type = LIGHT_POINT;
        l->pos = Camera::Pos();
        l->color = color(std::rand() % 255, std::rand() % 255, std::rand() % 255, 255);
        l->intensities = {0.01, 1.0, 1.0};
        scene.AddLight(l);
        Log::logf("Adding light. Now there are %d!", scene.Lights().size());
        auto *cMat = new Material(*colorShader, "cube");
        auto c = new Cube(*cMat);
        c->pos = Camera::Pos();
        c->scale = 0.3f;
        scene.AddObject(c);
        lightToObj[l] = c;
        Log::logf("Adding object. Now there are %d!", scene.Objects().size());
      }

      renderer.Draw(scene);

      Renderer::EndFrame(Application::window);
    }

    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}