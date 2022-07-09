#include "Scene.h"
#include "Renderer.h"
#include <algorithm>

void Scene::AddObject(Object *object) {
  objects.push_back(object);
}

void Scene::RemoveObject(Object *object) {
  auto toRemove = std::remove(objects.begin(), objects.end(), object);
  objects.erase(toRemove, objects.end());
}

void Scene::AddLight(LightSource *light) {
  if (lights.size() < MAX_LIGHTS)
    lights.push_back(light);
}


