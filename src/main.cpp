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
#include "graphics/CubeTexture.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

glm::vec4 color(int i, int i1, int i2);

using std::cos, std::sin, std::acos;

void SetupScene(Scene &scene, std::unordered_map<LightSource *, Object *> &lightToObj) {
  auto m = new Material("Ground");
  m->diffuseTex = make_shared<Texture>("assets/images/ground.jpg");
  m->matData.diffuseColor = glm::vec3(1.0f);
  m->matData.specColor = glm::vec3(0.0f);
  Cube *ground = new Cube(*m);
  ground->SetPos(glm::vec3(0, -2, 0.0f))
          ->SetScale(glm::vec3(200, 0.1, 200))
          ->SetTexScale(100.0f);
  scene.AddObject(ground);

  Mesh *cube = new Mesh("assets/mesh/cube.obj");
  cube->SetPos(glm::vec3(0, -0.5, 0))
          ->SetScale(1);
  scene.AddObject(cube);

  auto dirLight = new LightSource();
  dirLight->type = LIGHT_DIR;
  dirLight->pos = glm::vec3(-1, 1, -1);
  dirLight->pos *= 10;
  dirLight->dir = glm::normalize(-dirLight->pos);
  dirLight->intensities = glm::vec3(0.12f, 0.3f, 0.3f);
  dirLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
  scene.AddLight(dirLight);

//  auto pointLight = new LightSource();
//  pointLight->type = LIGHT_POINT;
//  pointLight->pos = glm::vec3(0.0f, 3.0f, -5.0f);
//  pointLight->intensities = glm::vec3(0.01f, 0.8f, 1.0f);
//  pointLight->color = glm::vec3(0.8f, 1.0f, 0.8f);
//  scene.AddLight(pointLight);
//
//  auto flatColor = new Material("flatColor");
//  auto lightCube = new Cube(*flatColor);
//  lightCube->SetPos(pointLight->pos)
//          ->SetScale(0.3);
//  scene.AddObject(lightCube);
//  lightToObj[pointLight] = lightCube;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  {
    Log::file("log.txt");
    ImGui_ImplWin32_EnableDpiAwareness();
    Application::window = NewWindow(hInstance, "Test window", INIT_WIDTH, INIT_HEIGHT);
    Application::Init();
    Log::logf("Version: %s", glGetString(GL_VERSION));
    Log::logf("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    Shader::CreateShader("color");
    Shader::CreateShader("light");
    Shader::CreateShader("shadow");
    Shader *screenShader = Shader::CreateShader("screen");
    Shader *skyShader = Shader::CreateShader("skybox");

    CubeTexture skyboxTex("assets/images/skybox/", {"right.jpg", "left.jpg", "top.jpg",
                                                    "bottom.jpg", "front.jpg", "back.jpg"});
    Cube skyCube(Material::DEFAULT);
    skyCube.SetScale(100);

    Scene scene;
    unordered_map<LightSource *, Object *> lightToObj;

    SetupScene(scene, lightToObj);

    const Renderer &renderer = *Application::renderer;

    int framesSinceAdd = 30;

    Material screenMat("ScreenMat");
    Square screen(screenMat);

    Application::frameBuf.push_back(new Framebuffer(4));
    auto &frameBuf = *Application::frameBuf[Application::frameBuf.size() - 1];
    frameBuf.AddTextureAttachment(GL_DEPTH_ATTACHMENT, Application::window->width, Application::window->height);
    frameBuf.AddTextureAttachment(GL_COLOR_ATTACHMENT0, Application::window->width, Application::window->height);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      Log::logf("ERROR: Unable to set up framebuffer");
    }

    Application::frameBuf.push_back(new Framebuffer());
    auto &shadowMap = *Application::frameBuf[Application::frameBuf.size() - 1];
    shadowMap.AddTextureAttachment(GL_DEPTH_ATTACHMENT, 1024, 1024);
    shadowMap.resizeToScreen = false;
    shadowMap.DisableColor();

    bool postProcess = false;
    // --- MAIN LOOP
    while (true) {
      framesSinceAdd++;
      HandleWindowMessage();
      Application::Update();
      if (!Application::running) {
        break;
      }

      // SHADOW MAP
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      glViewport(0, 0, 1024, 1024);
      glActiveTexture(GL_TEXTURE0);
      shadowMap.Bind();
      glClear(GL_DEPTH_BUFFER_BIT);
      float near_plane = 1.0f, far_plane = 30.0f;
      glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
      glm::mat4 lightView = glm::lookAt(glm::vec3(scene.Lights()[0]->pos),
                                        glm::vec3(0, 0, 0),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
      glm::mat4 lightSpaceTransform = lightProjection * lightView;
      Shader *shadowShader = Shader::LoadShader("shadow");
      shadowShader->SetUniform("lightTransform", UM4f, &lightSpaceTransform);

      shadowMap.BindTexture(GL_DEPTH_ATTACHMENT);

      TransformationData t{};
      t.vp = glm::mat4(1.0f);

      for (auto object: scene.Objects()) {
        t.model = object->Model();
        Shader::SetGlobalUniform("Transformations", (char *) &t, sizeof(TransformationData));
        object->Draw(*shadowShader);
      }

      // MAIN RENDER
      // Set lightTransform in lighting shader for render
      Shader::LoadShader("light")->SetUniform("u_lightTransform", UM4f, &lightSpaceTransform);
      int depthTexSlot = 31;
      glActiveTexture(GL_TEXTURE0 + depthTexSlot);
      shadowMap.BindTexture(GL_DEPTH_ATTACHMENT);
      Shader::LoadShader("light")->SetUniform("u_lightDepthMap", U1i, &depthTexSlot);
      frameBuf.Bind();
      glViewport(0, 0, Application::window->width, Application::window->height);
      Renderer::NewFrame();

      // SKYBOX
      glDisable(GL_CULL_FACE);
      glDepthMask(GL_FALSE);
      skyShader->Bind();
      TransformationData skyTransforms{};
      auto view = glm::mat4(glm::mat3(Camera::ViewMatrix()));
      skyTransforms.vp = renderer.proj * view;
      skyTransforms.model = glm::mat4(1.0f); // Identity
      Shader::SetGlobalUniform("Transformations", (char *) &skyTransforms, sizeof(TransformationData));
      int skyTexSlot = 18;
      skyboxTex.Bind(skyTexSlot);
      skyShader->SetUniform("u_cubeTex", U1i, &skyTexSlot);
      skyCube.Draw(*skyShader);
      glDepthMask(GL_TRUE);
      glEnable(GL_CULL_FACE);

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
        if (ImGui::TreeNode("Objects")) {
          int idx = 0;
          for (auto sceneObj: scene.Objects()) {
            std::stringstream name;
            name << idx++;
            if (ImGui::TreeNode(name.str().c_str())) {
              glm::vec3 s = sceneObj->Scale();
              glm::vec3 p = sceneObj->Pos();
              glm::vec3 r = sceneObj->Rot();
              float texScale = sceneObj->TexScale();
              if (ImGui::SliderFloat("Texture Scale", &texScale, 0.5, 30))
                sceneObj->SetTexScale(texScale);
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
      for (auto l: scene.Lights()) {
        auto o = (Primitive *) lightToObj[l];
        if (o && l->type == LIGHT_POINT) {
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
        auto *cMat = new Material("lightcube");
        auto c = new Cube(*cMat);
        c->SetPos(Camera::Pos())
                ->SetScale(0.3f)
                ->useLighting = false;
        scene.AddObject(c);
        lightToObj[l] = c;
        Log::logf("Adding object. Now there are %d!", scene.Objects().size());
      }

      renderer.Draw(scene);

      // SCREEN DRAW
      // Render scene framebuffer to screen
      frameBuf.Unbind();
      screenShader->Bind();
      renderer.Clear();
      glActiveTexture(GL_TEXTURE9);
      frameBuf.BindTexture(GL_COLOR_ATTACHMENT0);
      int texSlot = 9;
      screenShader->SetUniform("u_screenTexture", U1i, &texSlot);
      screenShader->SetUniform("width", U1i, &Application::window->width);
      screenShader->SetUniform("height", U1i, &Application::window->height);
      int pp = (int) postProcess;
      screenShader->SetUniform("postProcess", U1i, &pp);
      screenShader->SetUniform("samples", U1i, &frameBuf.samples);
      glDisable(GL_DEPTH_TEST);
      screen.Draw(*screenShader);

      Renderer::EndFrame(Application::window);
    }

    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}