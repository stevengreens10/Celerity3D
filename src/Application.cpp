#include "Application.h"
#include "Input.h"
#include "Camera.h"

void Application::Init() {
  Application::renderer = new Renderer();
  Camera::Translate(glm::vec3(157.22, 0, -183));
  Camera::Rotate(90.0f, 0.0f);
  renderer->Init(window);
}

void Application::Update() {
  float speed = 1.0f;

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