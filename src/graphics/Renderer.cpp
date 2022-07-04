#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Renderer.h"

void Renderer::Clear() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
}

void Renderer::Draw(const Shader &sh, const VertexArray &vb, const IndexBuffer &ib) const {
  sh.Bind();
  vb.Bind();
  ib.Bind();
  glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawImGui() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  bool show_demo_window = true;
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

  // Rendering
  ImGui::Render();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

glm::mat4 Renderer::GetMVP(int width, int height) {
  glm::mat4 proj = glm::ortho(0.0f, (float) width, 0.0f, (float) height, -1.0f, 1.0f);
  // Translate camera to the left
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(100, 0, 0));
//  glm::mat4 view = glm::rotate(glm::mat4(1.0f), 10.0f, glm::vec3(0.0, 1.0, 0.0));
  // Translate object up 200 right 200
//  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 mvp = proj * view * model;

  return mvp;
}
