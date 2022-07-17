#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <memory>

#include "Renderer.h"
#include "../log.h"
#include "GL/wglew.h"
#include "object/primitive/Cube.h"
#include "../Camera.h"
#include "../Application.h"

void Renderer::Init(Window *win) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&Log::GLDebugMessageCallback, nullptr);

  wglSwapIntervalEXT(1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearDepth(1.0f);                                 // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
  //TODO: Proper gamma correction
  //  glEnable(GL_FRAMEBUFFER_SRGB);

  SetProjection(win->width, win->height);

  Cube::InitBuffers();
  Square::InitBuffers();

  BufferLayout transformationLayout;
  transformationLayout.Push<glm::mat4>(2);
  Shader::CreateGlobalUniform("Transformations", transformationLayout, 1);

  BufferLayout sceneLayout;
  // Camera Pos
  sceneLayout.Push<glm::vec3>(1);
  // Num lights
  sceneLayout.Push<int>(1);

  // LightSource struct
  BufferLayout lightLayout;
  lightLayout.Push<int>(1); // Type
  lightLayout.Push<int>(1); // idx
  lightLayout.Push<glm::mat4>(6); // space transform
  lightLayout.Push<glm::vec3>(5);
  sceneLayout.PushStruct(lightLayout, MAX_LIGHTS);

  Shader::CreateShaderStorageBuffer("Scene", sceneLayout, 2);
#ifdef IMGUI
  Renderer::InitImGui(win);
#endif

  renderBuf = std::make_unique<Framebuffer>(1);
  renderBuf->CreateTextureAttachment(GL_DEPTH_ATTACHMENT, Application::window->width, Application::window->height);
  renderBuf->CreateTextureAttachment(GL_COLOR_ATTACHMENT0, Application::window->width, Application::window->height);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    Log::logf("ERROR: Unable to set up framebuffer");
  }

  shadowBuf = std::make_unique<Framebuffer>(1);
  shadowBuf->CreateTextureAttachment(GL_DEPTH_ATTACHMENT, SHADOW_WIDTH, SHADOW_HEIGHT);
  shadowBuf->resizeToScreen = false;
  shadowBuf->DisableColor();

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    Log::logf("ERROR: Unable to set up framebuffer");
  }


  glGenTextures(1, &cubeTexArrId);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeTexArrId);
  glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT,
                 MAX_SHADOW_LIGHTS * 6);

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

  glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                  nullptr);

  shadowBuf->Bind();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeTexArrId, 0);

  skyCube = std::make_unique<Cube>(Material::DEFAULT);

  screen = std::make_unique<Square>(Material::DEFAULT);

  float lnear = 1.0f;
  lightFarPlane = 50.0f;
  float aspect = (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT;
  lightProj = glm::perspective(glm::radians(90.0f), aspect, lnear, lightFarPlane);
  lightOrtho = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lnear, 30.0f);
  Shader::LoadShader("shadow")->SetUniform("u_farPlane", U1f, &lightFarPlane);

}

void Renderer::Cleanup(Window *win) {
#ifdef IMGUI
  ImGui_ImplWin32_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
#endif
  wglDeleteContext(win->renderContext);
}

void Renderer::EndFrame(Window *win) {
#ifdef IMGUI
  Renderer::UpdateImGui(win);
#endif
  SwapBuffers(win->deviceContext);
}

void Renderer::Clear() {
  glClearColor(0.28f, 0.28f, 0.28f, 1.0f);               // Set Background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
}

void Renderer::NewFrame() {
//  Clear();
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
#ifdef IMGUI
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
#endif
}

void Renderer::Draw(const Scene &s) const {
#pragma pack(push, 1)
  struct TransformationData {
      glm::mat4 vp;
      glm::mat4 model;
  };
  TransformationData t{};
#pragma pack(pop)

  Shader *shadowShader = Shader::LoadShader("shadow");
  Shader *lightShader = Shader::LoadShader("light");
  Shader *colorShader = Shader::LoadShader("color");
  Shader *skyShader = Shader::LoadShader("skybox");
  Shader *screenShader = Shader::LoadShader("screen");

  // Update light transforms
  for (auto light: s.Lights()) {
    if (light->type == LIGHT_POINT) {
      light->spaceTransform[0] = lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(1, 0, 0), {0, -1, 0});
      light->spaceTransform[1] = lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(-1, 0, 0), {0, -1, 0});
      light->spaceTransform[2] = lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, 1, 0), {0, 0, 1});
      light->spaceTransform[3] = lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, -1, 0), {0, 0, -1});
      light->spaceTransform[4] = lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, 0, 1), {0, -1, 0});
      light->spaceTransform[5] = lightProj * glm::lookAt(light->pos, light->pos + glm::vec3(0, 0, -1), {0, -1, 0});
    } else if (light->type == LIGHT_DIR) {
      light->spaceTransform[0] = lightOrtho * glm::lookAt(light->pos, light->pos + light->dir, {0, 1, 0});
    }
  }

  // Set scene buffer
  uint64_t bufSize = 16 + s.Lights().size() * sizeof(LightSource);
  char *buf = (char *) malloc(bufSize);
  ((glm::vec3 *) buf)[0] = Camera::Pos();
  ((int *) (buf + 12))[0] = (int) s.Lights().size();
  for (int i = 0; i < s.Lights().size(); i++) {
    auto offset = 16 + sizeof(LightSource) * i;
    ((LightSource *) (buf + offset))[0] = *s.Lights()[i];
  }
  Shader::SetShaderStorageBuffer("Scene", buf, bufSize);
  free(buf);

  // ---SHADOW PASSES---
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  shadowBuf->Bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeTexArrId);
  glClear(GL_DEPTH_BUFFER_BIT);

  t.vp = glm::mat4(1.0f);

  size_t shadowPasses = s.Lights().size() / 14 + 1;
  for (int i = 0; i < shadowPasses; i++) {
    for (auto object: s.Objects()) {
      t.model = object->Model();
      Shader::SetGlobalUniform("Transformations", (char *) &t, sizeof(TransformationData));
      object->Draw(*shadowShader);
    }
  }

  // ---SKYBOX---
  renderBuf->Bind();
  glViewport(0, 0, Application::window->width, Application::window->height);
  Clear();
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_FALSE);
  skyShader->Bind();
  TransformationData skyTransforms{};
  auto view = glm::mat4(glm::mat3(Camera::ViewMatrix()));
  skyTransforms.vp = proj * view;
  skyTransforms.model = glm::mat4(1.0f); // Identity
  Shader::SetGlobalUniform("Transformations", (char *) &skyTransforms, sizeof(TransformationData));
  int skyTexSlot = 18;
  s.skyboxTex->Bind(skyTexSlot);
  skyShader->SetUniform("u_cubeTex", U1i, &skyTexSlot);
  skyCube->Draw(*skyShader);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);


  // ---MAIN RENDER---
  Shader::LoadShader("light")->SetUniform("u_lightFarPlane", U1f, (void *) &lightFarPlane);
  int depthTexSlot = 31;
  glActiveTexture(GL_TEXTURE0 + depthTexSlot);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeTexArrId);
  Shader::LoadShader("light")->SetUniform("u_lightDepthMaps", U1i, &depthTexSlot);

  t.vp = proj * Camera::ViewMatrix();
  lightShader->Bind();
  for (auto object: s.Objects()) {
    t.model = object->Model();
    Shader::SetGlobalUniform("Transformations", (char *) &t, sizeof(TransformationData));
    float texScale = object->TexScale();
    lightShader->SetUniform("u_texScale", U1f, &texScale);
    if (object->useLighting)
      object->Draw(*lightShader);
    else
      object->Draw(*colorShader);
  }

  // --- SCREEN RENDER ---
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  int texSlot = 0;
  renderBuf->BindTexture(GL_COLOR_ATTACHMENT0, texSlot);
  screenShader->Bind();

  screenShader->SetUniform("u_screenTexture", U1i, &texSlot);
  screenShader->SetUniform("width", U1i, &Application::window->width);
  screenShader->SetUniform("height", U1i, &Application::window->height);
  int pp = (int) false;
  screenShader->SetUniform("postProcess", U1i, &pp);
  screenShader->SetUniform("samples", U1i, &renderBuf->samples);
  glDisable(GL_DEPTH_TEST);
  screen->Draw(*screenShader);
}

void Renderer::SetProjection(int width, int height) {
  proj = glm::perspective(glm::radians(59.0f), (float) width / (float) height, 0.1f, 1000.0f);
  if (renderBuf)
    renderBuf->Resize(width, height);
}

void Renderer::InitImGui(Window *win) {
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

  ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplWin32_InitForOpenGL(win->hWnd, win->renderContext);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void Renderer::UpdateImGui(Window *win) {
  // Rendering
  ImGui::Render();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    HDC backup_current_context = GetDC(win->hWnd);
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    wglMakeCurrent(backup_current_context, win->renderContext);
  }
}