#include "Application.h"
#include "Input.h"
#include "Camera.h"

void Application::Init() {

  Shader::CreateShader("color");
  Shader::CreateShader("light");
  Shader::CreateShader("shadow");
  Shader::CreateShader("screen");
  Shader::CreateShader("skybox");

  Application::renderer = new Renderer();
  Camera::Translate(glm::vec3(0, 0, -4));
  Camera::Rotate(90.0f, 0.0f);
  renderer->Init(window);
}

void Application::Update() {
  float speed = 0.1f;

  glm::vec3 horizontalDir = glm::normalize(glm::cross(glm::vec3(0, 1, 0), Camera::Dir())) * speed;
  if (Input::IsPressed('A')) {
    Camera::Translate(horizontalDir);
  }
  if (Input::IsPressed('D')) {
    Camera::Translate(-horizontalDir);
  }
  if (Input::IsPressed(' ')) {
    Camera::Translate({0, speed, 0});
  }
  if (Input::IsPressed(VK_SHIFT)) {
    Camera::Translate({0, -speed, 0});
  }
  if (Input::IsPressed('W')) {
    Camera::Translate(glm::normalize(glm::vec3(Camera::Dir().x, 0, Camera::Dir().z)) * speed);
  }
  if (Input::IsPressed('S')) {
    Camera::Translate(glm::normalize(glm::vec3(Camera::Dir().x, 0, Camera::Dir().z)) * -speed);
  }

}