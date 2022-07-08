#include "Input.h"

void Input::HandleKeyPressEvent(int key) {
  keysDown[key] = true;
}

void Input::HandleKeyReleaseEvent(int key) {
  keysDown[key] = false;
}

void Input::HandleMousePressEvent(int button) {
  keysDown[-button] = true;
}

void Input::HandleMouseReleaseEvent(int button) {
  keysDown[-button] = false;
}

void Input::HandleMouseMoveEvent(int x, int y) {
  mousePos = glm::vec2(x, y);
}

bool Input::IsPressed(int key) {
  return keysDown.contains(key) ? keysDown[key] : false;
}

bool Input::IsMousePressed(int button) {
  return IsPressed(-button);
}

glm::vec2 Input::MousePos() {
  return mousePos;
}
