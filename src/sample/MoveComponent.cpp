#include "MoveComponent.h"
#include "../Application.h"

void MoveComponent::Start() {}

void MoveComponent::Update() {
//  if (timer > 1.0f) {
  object->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * Application::deltaT);
//    timer = 0.0f;
//  } else {
//    timer += Application::deltaT;
//  }
}
