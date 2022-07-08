#include "Scene.h"
#include <algorithm>

void Scene::AddObject(Renderable *object) {
  objects.push_back(object);
}

void Scene::RemoveObject(Renderable *object) {
  auto toRemove = std::remove(objects.begin(), objects.end(), object);
  objects.erase(toRemove, objects.end());
}

void Scene::AddLight(LightSource *light) {
  lights.push_back(light);
}


