#include "Camera.h"

using std::cos, std::sin;

void Camera::Displace(int dx, int dy) {
  dy = -dy;
  float sens = 0.1f;

  float yawVal = (float) dx * sens;
  float pitchVal = (float) dy * sens;
  float newPitch = pitch + pitchVal;
  if (newPitch > 89.0f)
    newPitch = 89.0f;
  if (newPitch < -89.0f)
    newPitch = -89.0f;
  Rotate(yaw + yawVal, newPitch);
}

void Camera::Translate(glm::vec3 translation) {
  cameraPos += translation;
  updateViewMatrix();
}

void Camera::Rotate(float _yaw, float _pitch) {
  yaw = _yaw;
  pitch = _pitch;
  cameraDir.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  cameraDir.y = sin(glm::radians(_pitch));
  cameraDir.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  cameraDir = glm::normalize(cameraDir);
  updateViewMatrix();
}

void Camera::updateViewMatrix() {
  viewMatrix = glm::lookAt(cameraPos, cameraPos + glm::normalize(cameraDir), glm::vec3(0, 1, 0));
}
