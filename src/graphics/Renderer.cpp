#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Renderer.h"
#include "../log.h"
#include "GL/wglew.h"
#include "object/primitive/Cube.h"
#include "../Camera.h"

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

  // TODO: Render to framebuffer with multisample
  //  glEnable(GL_MULTISAMPLE);

  // TODO: Fix primitive vertices then re-enable
  //  glEnable(GL_CULL_FACE);
  //  glCullFace(GL_BACK);
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
  lightLayout.Push<int>(1);
  lightLayout.Push<glm::vec3>(5);
  sceneLayout.PushStruct(lightLayout, MAX_LIGHTS);

  Shader::CreateShaderStorageBuffer("Scene", sceneLayout, 2);
#ifdef IMGUI
  Renderer::InitImGui(win);
#endif
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
  Clear();
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

  t.vp = proj * Camera::ViewMatrix();
  for (auto object: s.Objects()) {
    t.model = object->Model();
    Shader::SetGlobalUniform("Transformations", (char *) &t, sizeof(TransformationData));
    object->Draw();
  }
}

void Renderer::SetProjection(int width, int height) {
  proj = glm::perspective(glm::radians(59.0f), (float) width / (float) height, 0.1f, 1000.0f);
  glViewport(0, 0, width, height);
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