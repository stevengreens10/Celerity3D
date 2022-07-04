#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Renderer.h"
#include "../log.h"
#include "GL/wglew.h"
#include "Cube.h"

void Renderer::Init(ApplicationWindow *win) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&GLDebugMessageCallback, nullptr);

  wglSwapIntervalEXT(1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  InitImGui(win);

  cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
  cameraTarget = glm::vec3(0);
  MAIN_SHADER = InitMainShader();
  Cube::InitBuffers();
}

void Renderer::Update(ApplicationWindow *win) {
  UpdateImGui(win);
  SwapBuffers(win->deviceContext);
}

void Renderer::Clear() const {
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
  r.shader->Bind();
  r.vao->Bind();
  r.ibo->Bind();
  r.texture->Bind();
  r.shader->SetUniform1i("u_Texture", 0);

  glm::mat4 view = GetView();
  glm::mat4 model = GetModel(glm::vec3(0.0f), r.scale, glm::vec3(100.0f, 100.0f, 0.0f), r.rot);
  r.shader->SetUniformMat4f("u_MVP", proj * view * model);

  glDrawElements(GL_TRIANGLES, r.ibo->getCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::SetProjection(int width, int height) {
  proj = glm::ortho(0.0f, (float) width, 0.0f, (float) height, 0.0f, 100.0f);
}

glm::mat4 Renderer::GetView() const {
  glm::vec3 cameraDir = glm::normalize(cameraPos - cameraTarget);

  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDir));
  glm::vec3 cameraUp = glm::cross(cameraDir, cameraRight);


  glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);
  return view;
}

glm::mat4 Renderer::GetModel(glm::vec3 pivot, float modelScale, glm::vec3 modelTranslate, glm::vec3 modelRotate) const {
// translate "pivot" to origin
  glm::mat4 ref2originM = glm::translate(glm::mat4(1.0f), -pivot);

// scale
  glm::mat4 scaleM = glm::scale(glm::mat4(1.0), glm::vec3(modelScale));

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
  ImGui_ImplWin32_EnableDpiAwareness();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

  ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplWin32_InitForOpenGL(win->window, win->renderContext);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void Renderer::UpdateImGui(ApplicationWindow *win) {
  // Rendering
  ImGui::Render();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    HDC backup_current_context = GetDC(win->window);
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    wglMakeCurrent(backup_current_context, win->renderContext);
  }
}

std::unique_ptr<Shader> Renderer::InitMainShader() {
  return std::make_unique<Shader>("assets/shaders/shader.vert", "assets/shaders/shader.frag");
}
