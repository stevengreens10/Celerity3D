#include "Object.h"


Object *Object::SetPos(glm::vec3 p) {
  pos = p;
  UpdateModel();
  return this;
}

Object *Object::Translate(glm::vec3 p) {
  pos += p;
  UpdateModel();
  return this;
}

Object *Object::SetScale(float s) {
  scale = glm::vec3(s, s, s);
  UpdateModel();
  return this;
}

Object *Object::SetScale(glm::vec3 s) {
  scale = s;
  UpdateModel();
  return this;
}

Object *Object::SetRot(glm::vec3 r) {
  rot = r;
  UpdateModel();
  return this;
}

void Object::UpdateModel() {
  // scale
  glm::mat4 scaleM = glm::scale(glm::mat4(1.0), scale);

  // rotate
  glm::mat4 rotationM(1.0);
  rotationM = glm::rotate(rotationM, glm::radians(rot.x), glm::vec3(1.0, 0.0, 0.0));
  rotationM = glm::rotate(rotationM, glm::radians(rot.y), glm::vec3(0.0, 1.0, 0.0));
  rotationM = glm::rotate(rotationM, glm::radians(rot.z), glm::vec3(0.0, 0.0, 1.0));

  // translate to "pos"
  glm::mat4 origin2posM = glm::translate(glm::mat4(1.0), pos);

  // concatenate matrices
  model = origin2posM * rotationM * scaleM;
}