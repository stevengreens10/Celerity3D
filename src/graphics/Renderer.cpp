#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Renderer.h"
#include "../log.h"
#include "GL/wglew.h"
#include "Cube.h"
#include "TriangularPrism.h"
#include "../Camera.h"

void Renderer::Init(Window *win) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&Log::GLDebugMessageCallback, nullptr);

  wglSwapIntervalEXT(1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
  glClearDepth(1.0f);                                 // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
  //TODO: Proper gamma correction
  //  glEnable(GL_FRAMEBUFFER_SRGB);

  // TODO: Render to framebuffer with multisample
  //  glEnable(GL_MULTISAMPLE);

  // TODO: Fix primitive vertices then re-enable
  //  glEnable(GL_CULL_FACE);
  //  glCullFace(GL_BACK);

  SetProjection(win->width, win->height);

  Cube::InitBuffers();
  TriangularPrism::InitBuffers();

  BufferLayout transformationLayout;
  transformationLayout.Push<glm::mat4>(2);
  Shader::CreateGlobalUniform("Transformations", transformationLayout, 1);

  BufferLayout sceneLayout;
  sceneLayout.Push<glm::vec3>(4);
  Shader::CreateGlobalUniform("Scene", sceneLayout, 2);
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

void Renderer::Draw(const Renderable &r) const {
  struct __attribute__ ((packed)) TransformationStruct {
      glm::mat4 vp;
      glm::mat4 model;
  };
  TransformationStruct t{};
  t.model = GetModel(glm::vec3(0.0f), r.scale, r.pos, r.rot);
  t.vp = proj * Camera::ViewMatrix();
  Shader::SetGlobalUniform("Transformations", (char *) &t);
  r.Draw();
}

void Renderer::SetProjection(int width, int height) {
  proj = glm::perspective(glm::radians(59.0f), (float) width / (float) height, 10.0f, 1000.0f);
  glViewport(0, 0, width, height);
}

glm::mat4 Renderer::GetModel(glm::vec3 pivot, float modelScale, glm::vec3 modelTranslate, glm::vec3 modelRotate) {
// translate "pivot" to origin
  glm::mat4 ref2originM = glm::translate(glm::mat4(1.0f), -pivot);

// scale
  glm::mat4 scaleM = glm::scale(glm::mat4(1.0), glm::vec3(modelScale, modelScale, modelScale));

// rotate
  glm::mat4 rotationM(1.0);
  rotationM = glm::rotate(rotationM, glm::radians(modelRotate.x), glm::vec3(1.0, 0.0, 0.0));
  rotationM = glm::rotate(rotationM, glm::radians(modelRotate.y), glm::vec3(0.0, 1.0, 0.0));
  rotationM = glm::rotate(rotationM, glm::radians(modelRotate.z), glm::vec3(0.0, 0.0, 1.0));

// translate to "pos"
  glm::mat4 origin2posM = glm::translate(glm::mat4(1.0), modelTranslate);

// concatenate matrices
  glm::mat4 model = origin2posM * rotationM * scaleM * ref2originM;
  return model;
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