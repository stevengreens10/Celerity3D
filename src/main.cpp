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
#include "graphics/Framebuffer.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

glm::vec4 color(int i, int i1, int i2);

using std::cos, std::sin, std::acos;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  {
    Log::file("log.txt");
    ImGui_ImplWin32_EnableDpiAwareness();
    Application::window = NewWindow(hInstance, "Test window", INIT_WIDTH, INIT_HEIGHT);
    Application::Init();
    Log::logf("Version: %s", glGetString(GL_VERSION));
    Log::logf("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    Shader *colorShader = Shader::CreateShader("color");
    Shader *screenShader = Shader::CreateShader("screen");
    Shader::CreateShader("light");

    Scene scene;

    Mesh cube("assets/mesh/cube.obj");
    cube.SetPos(glm::vec3(0, 0, 0.0f))
            ->SetScale(1);
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
    lightCube.SetPos(light.pos)
            ->SetScale(0.3);
    scene.AddObject(&lightCube);
    lightToObj[&light] = &lightCube;

    const Renderer &renderer = *Application::renderer;

    float angle = 0;
    int framesSinceAdd = 30;

    Material screenMat(*screenShader, "ScreenMat");
    Square screen(screenMat);

    Application::frameBuf = new Framebuffer(4);
    auto &frameBuf = *Application::frameBuf;
    frameBuf.AddTextureAttachment(GL_DEPTH_STENCIL_ATTACHMENT, Application::window->width, Application::window->height);
    frameBuf.AddTextureAttachment(GL_COLOR_ATTACHMENT0, Application::window->width, Application::window->height);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      Log::logf("ERROR: Unable to set up framebuffer");
    }

    bool postProcess = false;
    // --- MAIN LOOP
    while (true) {
      framesSinceAdd++;
      HandleWindowMessage();
      Application::Update();
      if (!Application::running) {
        break;
      }

      frameBuf.Bind();
      glEnable(GL_DEPTH_TEST);
      Renderer::NewFrame();

#ifdef IMGUI
      ImGui::Begin("Debug");
      ImGui::Checkbox("Postprocess", &postProcess);
      if (ImGui::TreeNode("Scene")) {
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
        if (ImGui::TreeNode("Meshes")) {
          int idx = 0;
          for (auto sceneObj: scene.Objects()) {
            std::stringstream name;
            name << idx++;
            if (ImGui::TreeNode(name.str().c_str())) {
              glm::vec3 s = sceneObj->Scale();
              glm::vec3 p = sceneObj->Pos();
              glm::vec3 r = sceneObj->Rot();
              if (ImGui::SliderFloat3("Scale", &(s.x), 0, 10))
                sceneObj->SetScale(s);
              if (ImGui::SliderFloat3("Position", &(p.x), -20, 20))
                sceneObj->SetPos(p);
              if (ImGui::SliderFloat3("Rotation", &(r.x), 0, 359))
                sceneObj->SetRot(r);
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
        ImGui::TreePop();
      }
      ImGui::NewLine();
      ImGui::Text("(%f, %f, %f)", Camera::Pos().x, Camera::Pos().y, Camera::Pos().z);
      ImGui::Text("%.3f ms/frame (%.1f) FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
#endif

      // Map cube pos and color to light source
      angle += 0.75f;
      for (auto l: scene.Lights()) {
        auto o = (Primitive *) lightToObj[l];
        if (o) {
          o->SetPos(l->pos);
          o->material.get().matData.diffuseColor = l->color;
        }
      }

      // Add lights dynamically
      if (Input::IsPressed(VK_TAB) && framesSinceAdd >= 30) {
        framesSinceAdd = 0;
        auto l = new LightSource();
        l->type = LIGHT_POINT;
        l->pos = Camera::Pos() + glm::vec3(0, 0.5f, 0);
        l->color = color(rand() % 255, rand() % 255, rand() % 255, 255);
        l->intensities = {0.01, 1.0, 1.0};
        scene.AddLight(l);
        Log::logf("Adding light. Now there are %d!", scene.Lights().size());
        auto *cMat = new Material(*colorShader, "cube");
        auto c = new Cube(*cMat);
        c->SetPos(Camera::Pos());
        c->SetScale(0.3f);
        scene.AddObject(c);
        lightToObj[l] = c;
        Log::logf("Adding object. Now there are %d!", scene.Objects().size());
      }

      renderer.Draw(scene);

      // Render scene framebuffer to screen
      frameBuf.Unbind();
      screenShader->Bind();
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBuf.GetTexture(GL_COLOR_ATTACHMENT0));
      int texSlot = 0;
      screenShader->SetUniform("u_screenTexture", U1i, &texSlot);
      screenShader->SetUniform("width", U1i, &Application::window->width);
      screenShader->SetUniform("height", U1i, &Application::window->height);
      int pp = (int) postProcess;
      screenShader->SetUniform("postProcess", U1i, &pp);
      screenShader->SetUniform("samples", U1i, &frameBuf.samples);
      glDisable(GL_DEPTH_TEST);
      screen.Draw();

      Renderer::EndFrame(Application::window);
    }

    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}