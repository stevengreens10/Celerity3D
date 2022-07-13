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

// Render to ASCII stdout instead of screen
#define ASCII false
// Terminal character resolution for ASCII rendering
#define CHAR_WIDTH 172
#define CHAR_HEIGHT 43

glm::vec4 color(int i, int i1, int i2);

void SetupScene(Scene &scene, std::unordered_map<LightSource *, Object *> &lightToObj) {
  auto m = new Material("Ground");
  m->diffuseTex = Texture::Load("assets/images/ground.jpg", true);
  m->matData.diffuseColor = glm::vec3(1.0f);
  m->matData.specColor = glm::vec3(0.0f);
  Cube *ground = new Cube(*m);
  ground->SetPos(glm::vec3(0, -20, 0.0f))
          ->SetScale(glm::vec3(200, 0.1, 200))
          ->SetTexScale(100.0f);
  scene.AddObject(ground);

  Mesh *cube = new Mesh("assets/mesh/cube.obj");
  cube->SetPos(glm::vec3(0, -0.5, 0))
          ->SetScale(1);
  scene.AddObject(cube);

  auto wallMat = new Material("Wall");
  wallMat->matData.diffuseColor = glm::vec3(color(84, 157, 235, 255));
  wallMat->matData.specColor = glm::vec3(1.0f);

  float roomSize = 7.0f;

  scene.AddObject((new Cube(*wallMat))
                          ->SetPos({roomSize, 0, 0})
                          ->SetScale({0.1, roomSize, roomSize}));

  scene.AddObject((new Cube(*wallMat))
                          ->SetPos({-roomSize, 0, 0})
                          ->SetScale({0.1, roomSize, roomSize}));

  scene.AddObject((new Cube(*wallMat))
                          ->SetPos({0, 0, roomSize})
                          ->SetScale({roomSize, roomSize, 0}));

  scene.AddObject((new Cube(*wallMat))
                          ->SetPos({0, 0, -roomSize})
                          ->SetScale({roomSize, roomSize, 0}));

  scene.AddObject((new Cube(*wallMat))
                          ->SetPos({0, roomSize, 0})
                          ->SetScale({roomSize, 0, roomSize}));

  scene.AddObject((new Cube(*wallMat))
                          ->SetPos({0, -roomSize, 0})
                          ->SetScale({roomSize, 0, roomSize}));

  auto l = new LightSource();
  l->type = LIGHT_POINT;
  l->pos = glm::vec3(3, 0, 0);
  l->dir = glm::normalize(-l->pos);
  l->intensities = glm::vec3(0.12f, 1.0f, 0.3f);
  l->color = glm::vec3(1.0f, 1.0f, 1.0f);
  scene.AddLight(l);

  auto *cMat = new Material("lightcube");
  auto c = new Cube(*cMat);
  c->SetPos(Camera::Pos())
          ->SetScale(0.2f)
          ->useLighting = false;
  scene.AddObject(c);
  lightToObj[l] = c;

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
    Shader *shadowShader = Shader::CreateShader("shadow");
    Shader *screenShader = Shader::CreateShader("screen");
    Shader *skyShader = Shader::CreateShader("skybox");

    std::unique_ptr<CubeTexture> skyboxTex(CubeTexture::Load("assets/images/skybox/",
                                                             {"right.jpg", "left.jpg", "top.jpg",
                                                              "bottom.jpg", "front.jpg", "back.jpg"}));

#if ASCII == true
    Shader *compShader = Shader::CreateComputeShader("ascii");
    BufferLayout asciiLayout;
    uint32_t numInts = (CHAR_WIDTH * CHAR_HEIGHT);
    asciiLayout.Push<int>(numInts);
    uint32_t asciiBuf = Shader::CreateShaderStorageBuffer("ASCII", asciiLayout, 3);
    uint32_t *chars = (uint32_t *) glMapNamedBuffer(asciiBuf, GL_READ_ONLY);
#endif

    Cube skyCube(Material::DEFAULT);
    skyCube.SetScale(100);

    Scene scene;
    unordered_map<LightSource *, Object *> lightToObj;

    SetupScene(scene, lightToObj);

    const Renderer &renderer = *Application::renderer;

    int framesSinceAdd = 30;

    Material screenMat("ScreenMat");
    Square screen(screenMat);

    Application::frameBuf.push_back(new Framebuffer(1));
    auto &frameBuf = *Application::frameBuf[Application::frameBuf.size() - 1];
    frameBuf.CreateTextureAttachment(GL_DEPTH_ATTACHMENT, Application::window->width, Application::window->height);
    frameBuf.CreateTextureAttachment(GL_COLOR_ATTACHMENT0, Application::window->width, Application::window->height);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      Log::logf("ERROR: Unable to set up framebuffer");
    }

    Application::frameBuf.push_back(new Framebuffer());
    auto &shadowMap = *Application::frameBuf[Application::frameBuf.size() - 1];
//    shadowMap.CreateTextureAttachment(GL_DEPTH_ATTACHMENT, 1024, 1024);
    shadowMap.resizeToScreen = false;

    int SHADOW_WIDTH = 1024;
    int SHADOW_HEIGHT = 1024;

    auto depthCubeMap = new CubeTexture(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    depthCubeMap->Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    shadowMap.SetTextureAttachment(GL_DEPTH_ATTACHMENT, depthCubeMap);
    shadowMap.DisableColor();

    float lnear = 1.0f, lfar = 25.0f;
    float aspect = (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT;
    glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), aspect, lnear, lfar);
    shadowShader->SetUniform("u_farPlane", U1f, &lfar);

    bool postProcess = false;
    // --- MAIN LOOP
    while (true) {
      framesSinceAdd++;
      HandleWindowMessage();
      Application::Update();
      if (!Application::running) {
        break;
      }

      // ---SHADOW MAP---
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glActiveTexture(GL_TEXTURE0);
      shadowMap.Bind();
      glClear(GL_DEPTH_BUFFER_BIT);

      auto light = scene.Lights()[0];
      std::vector<glm::mat4> lightTransforms = {
              lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(1, 0, 0), {0, -1, 0}),
              lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(-1, 0, 0), {0, -1, 0}),
              lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, 1, 0), {0, 0, 1}),
              lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, -1, 0), {0, 0, -1}),
              lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, 0, 1), {0, -1, 0}),
              lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, 0, -1), {0, -1, 0})
      };

      shadowShader->SetUniform("u_lightTransforms", UM4f, &lightTransforms[0], 6);
      shadowShader->SetUniform("u_lightPos", U3f, &(light->pos.x));

      shadowMap.BindTexture(GL_DEPTH_ATTACHMENT, 0);

      TransformationData t{};
      t.vp = glm::mat4(1.0f);

      for (auto object: scene.Objects()) {
        t.model = object->Model();
        Shader::SetGlobalUniform("Transformations", (char *) &t, sizeof(TransformationData));
        object->Draw(*shadowShader);
      }

      // Set lightTransform in lighting shader for render
      int depthTexSlot = 31;
      shadowMap.BindTexture(GL_DEPTH_ATTACHMENT, depthTexSlot);
      Shader::LoadShader("light")->SetUniform("u_lightDepthMap", U1i, &depthTexSlot);
      Shader::LoadShader("light")->SetUniform("u_lightFarPlane", U1f, &lfar);
      frameBuf.Bind();
      glViewport(0, 0, Application::window->width, Application::window->height);
      Renderer::NewFrame();

      // ---SKYBOX---
      glDisable(GL_CULL_FACE);
      glDepthMask(GL_FALSE);
      skyShader->Bind();
      TransformationData skyTransforms{};
      auto view = glm::mat4(glm::mat3(Camera::ViewMatrix()));
      skyTransforms.vp = renderer.proj * view;
      skyTransforms.model = glm::mat4(1.0f); // Identity
      Shader::SetGlobalUniform("Transformations", (char *) &skyTransforms, sizeof(TransformationData));
      int skyTexSlot = 18;
      skyboxTex->Bind(skyTexSlot);
      skyShader->SetUniform("u_cubeTex", U1i, &skyTexSlot);
      skyCube.Draw(*skyShader);
      glDepthMask(GL_TRUE);
      glEnable(GL_CULL_FACE);

      // ---MAIN RENDER---
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

      frameBuf.Unbind();
      Renderer::Clear();

      int texSlot = 0;
      frameBuf.BindTexture(GL_COLOR_ATTACHMENT0, texSlot);
#if ASCII == true
      // ---ASCII Compute Shader---
      int imageSlot = 0;
      compShader->Bind();
      glBindImageTexture(imageSlot, frameBuf.GetTexture(GL_COLOR_ATTACHMENT0)->id, 0,
                         GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
      glDispatchCompute(CHAR_WIDTH, CHAR_HEIGHT, 1);

      // -- TERMINAL OUTPUT --
      // make sure writing to image has finished before read
      glMemoryBarrier(GL_ALL_BARRIER_BITS);

      printf("\033[H");
      char lastColor = 0;
      for (int i = 0; i < (CHAR_WIDTH * CHAR_HEIGHT); i++) {
        char color = (chars[i] & 0xff00) >> 8;
        if (lastColor != color) {
          printf("\033[38;5;%dm", color);
          lastColor = color;
        }
        putc((char) (chars[i] & 0xff), stdout);
      }
      fflush(stdout);
#else
      // ---SCREEN DRAW---
      screenShader->Bind();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      screenShader->SetUniform("u_screenTexture", U1i, &texSlot);
      screenShader->SetUniform("width", U1i, &Application::window->width);
      screenShader->SetUniform("height", U1i, &Application::window->height);
      int pp = (int) postProcess;
      screenShader->SetUniform("postProcess", U1i, &pp);
      screenShader->SetUniform("samples", U1i, &frameBuf.samples);
      glDisable(GL_DEPTH_TEST);
      screen.Draw(*screenShader);
#endif
      Renderer::EndFrame(Application::window);
    }
    // Cleanup
    Renderer::Cleanup(Application::window);
  }
  Log::close();

  return 0;
}