#include <chrono>
#include <windows.h>
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
#include "World.h"
#include "graphics/CubeTexture.h"

#include "PxPhysicsAPI.h"
#include "extensions/PxSimpleFactory.h"
#include "physx/PhysXUtil.h"
#include "physx/Physics.h"
#include "sample/MoveComponent.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

// Render to ASCII stdout instead of screen
#define ASCII false
// Terminal character resolution for ASCII rendering
#define CHAR_WIDTH 172
#define CHAR_HEIGHT 43

glm::vec4 color(int i, int i1, int i2);

void SetupScene(World &world, std::unordered_map<LightSource *, Object *> &lightToObj) {
  Log::logf("Populating world");
  auto m = new Material("Ground");
  m->diffuseTex = Texture::Load("assets/images/ground.jpg", true);
  m->matData.diffuseColor = glm::vec3(1.0f);
  m->matData.specColor = glm::vec3(0.0f);
  Cube *ground = new Cube(*m);
  const glm::vec groundPos = glm::vec3(0, -2, 0.0f);
  const glm::vec groundScale = glm::vec3(200, 0.1, 200);
  ground->SetPos(groundPos)
          ->SetScale(groundScale)
          ->SetTexScale(100.0f);

  PhysicsMaterial physMat{0.9, 0.9, 0.1};
  physx::PxRigidActor *groundActor = Physics::createRigidStatic(groundPos, glm::vec3(0.0f), groundScale, physMat);
  ground->SetPhysicsActor(groundActor);
  world.AddObject(ground);

  Mesh *cube = new Mesh("assets/mesh/cube.obj");
  const glm::vec cubePos = glm::vec3(0, 5, 0);
  glm::vec3 cubeRot(60.0f, 0.0f, 0.0f);
  cube->SetPos(cubePos)->SetRot(cubeRot)->SetScale(1);

  physx::PxRigidDynamic *cubeActor = Physics::createRigidDynamic(cubePos, cubeRot, glm::vec3(1.0f),
                                                                 physMat);
  cube->SetPhysicsActor(cubeActor);
  world.AddObject(cube);

  float z = 5;
  for (int k = 0; k < 2; k++) {
    float x = 5;
    for (int j = 0; j < 10; j++) {
      float y = -1.4;
      for (int i = 0; i < 10; i++) {
        const glm::vec p = glm::vec3(x, y, z);
        auto *cMat = new Material("cubemat");
        cMat->matData.diffuseColor = color(rand() % 255, rand() % 255, rand() % 255, 255);
        Cube *c = new Cube(*cMat);
        c->SetPos(p)->SetScale(0.5);

        physx::PxRigidDynamic *rigidDynamic = Physics::createRigidDynamic(p, glm::vec3(0.0f), glm::vec3(0.5f),
                                                                          physMat);
        c->SetPhysicsActor(rigidDynamic);
        world.AddObject(c);
        y += 1.0;
      }
      x += 1.0;
    }
    z++;
  }


  auto wallMat = new Material("Wall");
  wallMat->matData.diffuseColor = glm::vec3(color(84, 157, 235, 255));
  wallMat->matData.specColor = glm::vec3(1.0f);

  /*float roomSize = 12.0f;

  world.AddObject((new Cube(*wallMat))
                          ->SetPos({roomSize, 0, 0})
                          ->SetScale({0.1, roomSize, roomSize}));

  world.AddObject((new Cube(*wallMat))
                          ->SetPos({-roomSize, 0, 0})
                          ->SetScale({0.1, roomSize, roomSize}));

  world.AddObject((new Cube(*wallMat))
                          ->SetPos({0, 0, roomSize})
                          ->SetScale({roomSize, roomSize, 0.1}));

  world.AddObject((new Cube(*wallMat))
                          ->SetPos({0, 0, -roomSize})
                          ->SetScale({roomSize, roomSize, 0.1}));

  world.AddObject((new Cube(*wallMat))
                          ->SetPos({0, roomSize, 0})
                          ->SetScale({roomSize, 0.1, roomSize}));

  world.AddObject((new Cube(*wallMat))
                          ->SetPos({0, -roomSize, 0})
                          ->SetScale({roomSize, 0.1, roomSize}));*/

  /*auto l = new LightSource();
  l->type = LIGHT_DIR;
  l->idx = 0;
  l->pos = glm::vec3(1, 1, 1);
  l->pos *= 10;
  l->dir = glm::normalize(-l->pos);
  l->intensities = glm::vec3(0.12f, 1.0f, 0.3f);
  l->color = glm::vec3(1.0f, 1.0f, 1.0f);
  world.AddLight(l);

  auto *cMat = new Material("lightcube");
  auto c = new Cube(*cMat);
  c->SetPos(l->pos)
          ->SetScale(0.2f)
          ->useLighting = false;
  world.AddObject(nullptr, c);
  lightToObj[l] = c;*/

  Log::logf("Done populating world");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  {
    Log::file("log.txt");
    ImGui_ImplWin32_EnableDpiAwareness();
    Application::window = NewWindow(hInstance, "Test window", INIT_WIDTH, INIT_HEIGHT);
    Application::Init();
    Log::logf("Version: %s", glGetString(GL_VERSION));
    Log::logf("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    CubeTexture *skyboxTex(CubeTexture::Load("assets/images/skybox/",
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

    World world;
    unordered_map<LightSource *, Object *> lightToObj;

    bool simulatePhysics = false;
    Physics::init();
    SetupScene(world, lightToObj);

    world.skyboxTex = skyboxTex;

    Renderer &renderer = *Application::renderer;

    int framesSinceAdd = 30;
    // --- MAIN LOOP
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (true) {
      auto start = std::chrono::high_resolution_clock::now();
      framesSinceAdd++;
      HandleWindowMessage();
      Application::Update();
      if (!Application::running) {
        break;
      }

      renderer.NewFrame();
      Log::debugf("Time at start loop: %f ms",
                  std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());

      start = std::chrono::high_resolution_clock::now();
#ifdef IMGUI
      ImGui::Begin("Debug");
//      ImGui::Checkbox("Postprocess", &postProcess);
      ImGui::Checkbox("Do Physic", &simulatePhysics);
      if (ImGui::TreeNode("Scene")) {
        if (ImGui::TreeNode("Lights")) {
          int idx = 0;
          for (auto sceneLight: world.Lights()) {
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
          for (auto sceneObj: world.Objects()) {
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
              physx::PxActor *pActor = sceneObj->PhysicsActor();
              if (pActor && pActor->getType() == physx::PxActorType::eRIGID_DYNAMIC) {
                ImGui::Text("Is sleeping? %s", ((physx::PxRigidDynamic *) (pActor))->isSleeping() ? "true" : "false");
                ImGui::NewLine();
              }
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
      Log::debugf("Time taken to GUI: %f ms",
                  std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());

      start = std::chrono::high_resolution_clock::now();
      // Map cube pos and color to light source
      for (auto l: world.Lights()) {
        auto o = (Primitive *) lightToObj[l];
        if (o) {
          o->SetPos(l->pos);
          o->material.get().matData.diffuseColor = l->color;
        }
      }
      Log::debugf("Time taken to update lights: %f ms",
                  std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());


      // Add lights dynamically
      if (Input::IsPressed(VK_TAB) && framesSinceAdd >= 30) {
        framesSinceAdd = 0;
        auto l = new LightSource();
        l->type = LIGHT_POINT;
        l->pos = Camera::Pos() + glm::vec3(0, 0.5f, 0);
        l->color = color(rand() % 255, rand() % 255, rand() % 255, 255);
        l->intensities = {0.01, 1.0, 1.0};
        l->idx = (int) world.Lights().size();
        world.AddLight(l);

        Log::logf("Adding light. Now there are %d!", world.Lights().size());
        auto *cMat = new Material("lightcube");
        auto c = new Cube(*cMat);
        c->SetPos(Camera::Pos())
                ->SetScale(0.3f)
                ->useLighting = false;

        world.AddObject(c);
        lightToObj[l] = c;
        Log::logf("Adding object. Now there are %d!", world.Objects().size());
      } else if (Input::IsPressed(VK_BACK) && framesSinceAdd >= 30) {
        framesSinceAdd = 0;
        auto *cMat = new Material("cubemat");
        cMat->matData.diffuseColor = color(rand() % 255, rand() % 255, rand() % 255, 255);
        auto c = new Cube(*cMat);
        const glm::vec3 cubePos = Camera::Pos() - glm::vec3(0, 0.5, 0);
        c->SetPos(cubePos)
                ->SetScale(0.5f);

        PhysicsMaterial physMat{0.5f, 0.5f, 0.1f};
        glm::vec3 velocity = glm::normalize(Camera::Dir()) * 30.0f;
        physx::PxRigidDynamic *cubeActor = Physics::createRigidDynamic(cubePos, glm::vec3(0.0f), glm::vec3(0.5f),
                                                                       physMat);
        cubeActor->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z));
        cubeActor->setAngularDamping(0.5f);
        c->SetPhysicsActor(cubeActor);
        world.AddObject(c);
      }

      start = std::chrono::high_resolution_clock::now();
      renderer.Draw(world);
      Log::debugf("Time taken to render: %f ms",
                  std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());

//      Renderer::Clear();

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
#endif

      start = std::chrono::high_resolution_clock::now();
      renderer.EndFrame(Application::window);
      Log::debugf("Time taken to end frame: %f ms",
                  std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());

      start = std::chrono::high_resolution_clock::now();

      auto endFrameTime = std::chrono::high_resolution_clock::now();
      Application::deltaT = std::chrono::duration<double, std::milli>(endFrameTime - lastFrameTime).count() / 1000.0f;
      if (simulatePhysics)
        world.Update(Application::deltaT);

      Log::debugf("Time taken to simulate physics: %f ms",
                  std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());

      lastFrameTime = endFrameTime;

    }
    // Cleanup
    Physics::cleanup();
    Renderer::Cleanup(Application::window);
  }
  Log::close();


  return 0;
}