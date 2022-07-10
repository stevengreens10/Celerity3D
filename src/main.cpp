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

    float quadVertices[] = {
            -1, -1, 0, 0,
            1, -1, 1, 0,
            1, 1, 1, 1,
            -1, 1, 0, 1
    };

    uint32_t quadIndices[] = {
            0, 1, 2,
            2, 3, 0
    };
    VertexArray quadVAO;
    quadVAO.Bind();

    VertexBuffer buf(quadVertices, sizeof(quadVertices));
    BufferLayout quadLayout;
    quadLayout.Push<float>(2);
    quadLayout.Push<float>(2);

    quadVAO.AddBuffer(buf, quadLayout);
    IndexBuffer idxBuf(quadIndices, 6);
    idxBuf.Bind();
    quadVAO.Unbind();
    idxBuf.Unbind();

    Application::frameBuf = new Framebuffer();
    auto &frameBuf = *Application::frameBuf;
    frameBuf.AddTextureAttachment(GL_DEPTH_STENCIL_ATTACHMENT, Application::window->width, Application::window->height);
    frameBuf.AddTextureAttachment(GL_COLOR_ATTACHMENT0, Application::window->width, Application::window->height);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      Log::logf("ERROR: Unable to set up framebuffer");
    }

    frameBuf.Unbind();
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
      ImGui::ShowDemoWindow();
      ImGui::Begin("Debug");
      if (ImGui::TreeNode("Lights")) {
        int idx = 0;
        for (auto sceneLight: scene.Lights()) {
          std::stringstream name;
          name << idx++;
          if (ImGui::TreeNode(name.str().c_str())) {
            ImGui::SliderFloat3("Position", &(sceneLight->pos.texSlot), -20, 20);
            ImGui::SliderFloat3("Color", &(sceneLight->color.texSlot), 0.0f, 1.0f);
            ImGui::SliderFloat3("Intensity", &(sceneLight->intensities.texSlot), 0, 2);
            ImGui::SliderFloat("attenConst", &(sceneLight->attenuation.texSlot), 0, 2);
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
            ImGui::SliderFloat3("Position", &(sceneObj->pos.texSlot), -20, 20);
            ImGui::SliderFloat3("Rotation", &(sceneObj->rot.texSlot), 0, 359);
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
      ImGui::Text("(%f, %f, %f)", Camera::Pos().texSlot, Camera::Pos().y, Camera::Pos().z);
      ImGui::Text("%.3f ms/frame (%.1f) FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
#endif

      // Make lights move around and map cube pos and color to light source
      angle += 0.75f;
      for (auto l: scene.Lights()) {
//        l->pos.texSlot += radius * std::cos(glm::radians(angle)) * 0.01;
//        l->pos.z += radius * std::sin(glm::radians(angle)) * 0.01;
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
        l->pos = Camera::Pos() + glm::vec3(0, 0.5f, 0);
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

      // Render scene framebuffer to screen
      frameBuf.Unbind();
      screenShader->Bind();
      glBindTexture(GL_TEXTURE_2D, frameBuf.GetTexture(GL_COLOR_ATTACHMENT0));
      int texSlot = 0;
      screenShader->SetUniform("u_screenTexture", U1i, &texSlot);
      screenShader->SetUniform("width", U1i, &Application::window->width);
      screenShader->SetUniform("height", U1i, &Application::window->height);
      glDisable(GL_DEPTH_TEST);
      quadVAO.Bind();
      glDrawElements(GL_TRIANGLES, (int) 6, GL_UNSIGNED_INT, nullptr);

      Renderer::EndFrame(Application::window);
    }

    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}