#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Renderer.h"
#include "../log.h"
#include "GL/wglew.h"
#include "Cube.h"
#include "TriangularPrism.h"

void Renderer::Init(ApplicationWindow *win) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&GLDebugMessageCallback, nullptr);

  wglSwapIntervalEXT(1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_MULTISAMPLE);

  Renderer::InitImGui(win);

  cameraPos = glm::vec3(0.0f, 0.0f, -250.0f);
  RotateCamera(90.0f, 0.0f);
  Cube::InitBuffers();
  TriangularPrism::InitBuffers();
}

void Renderer::Cleanup(ApplicationWindow *win) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
  ImGui_ImplWin32_Shutdown();
  wglDeleteContext(win->renderContext);
}

void Renderer::Update(ApplicationWindow *win) {
  Renderer::UpdateImGui(win);
  SwapBuffers(win->deviceContext);
}

void Renderer::Clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
}

void Renderer::NewFrame() {
  Clear();
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

void Renderer::Draw(const Renderable &r) const {
  glm::mat4 model = GetModel(glm::vec3(0.0f), r.scale, r.pos, r.rot);

  glm::mat4 vp = proj * view;

  // TODO: Share this across all shaders
  auto shader = Shader::LoadShader("light");
  shader->Bind();
  glUniformMatrix4fv(glGetUniformLocation(shader->rendererId, "u_VP"), 1, false, &(vp[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(shader->rendererId, "u_M"), 1, false, &(model[0][0]));

  r.Draw();
}

void Renderer::SetProjection(int width, int height) {
  proj = glm::perspective(glm::radians(59.0f), (float) width / (float) height, 0.1f, 10000.0f);

}

void Renderer::RotateCamera(float _yaw, float _pitch) {
  yaw = _yaw;
  pitch = _pitch;
  cameraDir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraDir.y = sin(glm::radians(pitch));
  cameraDir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraDir = glm::normalize(cameraDir);
  CalculateView();
}

void Renderer::CalculateView() {
  view = glm::lookAt(cameraPos, cameraPos + glm::normalize(cameraDir), glm::vec3(0, 1, 0));
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

void Renderer::InitImGui(ApplicationWindow *win) {
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

  ImGui_ImplWin32_InitForOpenGL(win->window, win->renderContext);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void Renderer::UpdateImGui(ApplicationWindow *win) {
  // Rendering
  ImGui::Render();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    HDC backup_current_context = GetDC(win->window);
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    wglMakeCurrent(backup_current_context, win->renderContext);
  }
}

void Renderer::DrawRenderableDebug(const std::string &name, Renderable *r) {
  ImGui::Begin(name.c_str());
  ImGui::SliderFloat("modelScale", &(r->scale), 50, 150);

  ImGui::SliderFloat3("modelX", &(r->pos.x), -300, 300);

  ImGui::SliderFloat3("modelRotate", &(r->rot.x), 0, 359);

  ImGui::Text("%.3f ms/frame (%.1f) FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}
