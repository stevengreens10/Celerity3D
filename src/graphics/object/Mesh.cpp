#include "Mesh.h"

#include <memory>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "LocalValueEscapesScope"
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/hash.hpp"
#include "../../log.h"

Mesh::Mesh(const string &fileName) {
  loadMesh(fileName);
}

void Mesh::Draw(Shader &shader) {
  vao->Bind();
  for (auto &mesh: meshes) {
    if (!mesh.shouldRender)
      continue;
    for (auto &it: mesh.matToIBO) {
      it.second->Bind();
      it.first->matData.useBump = useBump;
      it.first->Bind(shader);
      glDrawElements(GL_TRIANGLES, (int) it.second->getCount(), GL_UNSIGNED_INT, nullptr);
    }
  }
}

unordered_map<string, shared_ptr<Material>> Mesh::loadMats(const string &fileName) {
  Log::logf("Loading materials %s", fileName.c_str());
  ifstream f;
  unordered_map<string, shared_ptr<Material>> mats;
  f.open(fileName);
  if (!f) {
    return mats;
  }

  string line;
  string currentMat;
  while (getline(f, line)) {
    auto tokens = split(line, " ");
    if (tokens[0] == "newmtl") {
      currentMat = tokens[1];
      mats[currentMat] = make_shared<Material>(tokens[1]);
    } else if (tokens[0] == "Ns") {
      mats[currentMat]->matData.shininess = std::stof(tokens[1]);
    } else if (tokens[0] == "map_Ns") {
      mats[currentMat]->shininessTex = Texture::Load("assets/images/" + tokens[1], true);
    } else if (tokens[0] == "Ka") {
      mats[currentMat]->matData.ambientColor = sToVec3(tokens, 1);
    } else if (tokens[0] == "map_Ka") {
      mats[currentMat]->ambientTex = Texture::Load("assets/images/" + tokens[1], true);
    } else if (tokens[0] == "Kd") {
      mats[currentMat]->matData.diffuseColor = sToVec3(tokens, 1);
    } else if (tokens[0] == "map_Kd") {
      mats[currentMat]->diffuseTex = Texture::Load("assets/images/" + tokens[1], true);
    } else if (tokens[0] == "map_Bump") {
      mats[currentMat]->bumpTex = Texture::Load("assets/images/" + tokens[1], true);
    } else if (tokens[0] == "Ks") {
      mats[currentMat]->matData.specColor = sToVec3(tokens, 1);
    } else if (tokens[0] == "map_Ks") {
      mats[currentMat]->specTex = Texture::Load("assets/images/" + tokens[1], true);
    } else if (tokens[0] == "Ke") {
      // TODO: Not using this yet
      Log::debugf("Don't support Ke yet in MTL");
    } else if (tokens[0] == "Ni") {
      mats[currentMat]->matData.refractionIndex = std::stof(tokens[1]);
    } else if (tokens[0] == "d") {
      mats[currentMat]->matData.alpha = std::stof(tokens[1]);
    } else if (tokens[0] == "illum") {
      // TODO: Not using this yet
      Log::debugf("Don't support illum yet in MTL");
    } else if (tokens[0] != "#" && !tokens[0].empty()) {
      Log::debugf("Unsupported line in MTL: %s", tokens[0].c_str());
    }
  }

  return mats;
}

bool Mesh::loadMesh(const string &fileName) {
  // Turn back... Don't read this code for your own sake.
  Log::logf("Loading mesh %s", fileName.c_str());
  ifstream f;
  f.open(fileName);
  if (!f) {
    return false;
  }

  string line;
  unordered_map<string, shared_ptr<Material>> materials;
  vector<glm::vec3> positions;
  vector<glm::vec2> uvCoords;
  vector<glm::vec3> normals;
  typedef glm::vec3 TriIdxVals;
  typedef tuple<TriIdxVals, TriIdxVals, TriIdxVals> TriTriplet;

  // Used to build vertex buffer
  vector<Vertex> vertices;
  // Maps position/uv/normal triplet to vertices index
  unordered_map<TriIdxVals, uint32_t> triVertexMap{};

  /* Maps submesh to material to tri array
    e.g.:

    Mesh
      -> Material
        -> Tris[]
      -> Material
      ...
    */
  vector<unordered_map<shared_ptr<Material>, vector<TriTriplet>>> triIndices;
  int meshIdx = -1;
  shared_ptr<Material> currentMat;
  while (getline(f, line)) {
    auto tokens = split(line, " ");
    if (tokens[0] == "mtllib") {
      materials = loadMats("assets/mesh/" + tokens[1]);
    } else if (tokens[0] == "v") {
      positions.push_back(sToVec3(tokens, 1));
    } else if (tokens[0] == "vt") {
      uvCoords.push_back(sToVec2(tokens, 1));
    } else if (tokens[0] == "vn") {
      normals.push_back(sToVec3(tokens, 1));
    } else if (tokens[0] == "usemtl") {
      currentMat = materials[tokens[1]];
      meshes[meshIdx].matToIBO[currentMat] = nullptr;
    } else if (tokens[0] == "f") {
      vector<TriIdxVals> faceVec;
      for (int i = 1; i < tokens.size(); i++) {
        auto faceTokens = split(tokens[i], "/");
        TriIdxVals idxVals = sToVec3(faceTokens, 0);
        faceVec.push_back(idxVals);

        if (!triVertexMap.contains(idxVals)) {
          triVertexMap[idxVals] = (int) vertices.size();
          // Default to 0 if not provided
          auto pos = glm::vec3(0.0f);
          if (positions.size() > (int) idxVals.x - 1)
            pos = positions[(int) idxVals.x - 1];

          auto uv = glm::vec2(0.0f);
          if (uvCoords.size() > (int) idxVals.y - 1)
            uv = uvCoords[(int) idxVals.y - 1];

          auto normal = glm::vec3(0.0f);
          if (normals.size() > (int) idxVals.z - 1)
            normal = normals[(int) idxVals.z - 1];

          vertices.push_back({pos, normal, uv});
        }

      }
      // manually generate a triangle-fan
      for (int x = 1; x < (faceVec.size() - 1); x++) {
        triIndices[meshIdx][currentMat].emplace_back(faceVec[0], faceVec[x], faceVec[x + 1]);
      }
    } else if (tokens[0] == "o") {
      meshIdx++;
      meshes.push_back({tokens[1]});
      // Make room for next mesh data
      triIndices.emplace_back();
    }
  }

  vao = std::make_unique<VertexArray>();
  for (int n = 0; n < meshes.size(); n++) {
    auto &meshData = meshes[n];
    for (const auto &it: meshData.matToIBO) {
      // Tris specific to this material
      auto matTris = triIndices[n][it.first];
      uint32_t numIndices = matTris.size() * 3;
      std::vector<uint32_t> indices(numIndices);

      // For each triangle
      for (int i = 0; i < matTris.size(); i++) {
        // 3-Tuple of pos/uv/normal idx triplet
        // (One for each vertex of triangle)
        auto t = matTris[i];
        int idx = i * 3;
        // Map pos/uv/normal idx triplet to index in vertex buffer
        indices[idx] = triVertexMap[get<0>(t)];
        indices[idx + 1] = triVertexMap[get<1>(t)];
        indices[idx + 2] = triVertexMap[get<2>(t)];
      }

      calculateTangents(vertices, indices);
      meshData.matToIBO[it.first] = std::make_shared<IndexBuffer>(
              &indices[0], numIndices);
    }
  }

  BufferLayout layout;
  layout.Push<float>(3); // Pos
  layout.Push<float>(3); // Normal
  layout.Push<float>(2); // UV
  layout.Push<float>(3); // Tangent
  VertexBuffer vbuf(&vertices[0], vertices.size() * sizeof(Vertex));
  vao->AddBuffer(vbuf, layout);

  Log::logf("Mesh loaded!");
  return true;
}

#pragma clang diagnostic pop