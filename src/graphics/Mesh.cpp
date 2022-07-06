#include "Mesh.h"

#include <memory>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "LocalValueEscapesScope"
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/hash.hpp"

Mesh::Mesh(const string &fileName, Material &m) : Renderable(reference_wrapper<Material>(m)) {
  loadMesh(fileName);
}

void Mesh::Draw() const {
  for (auto &mesh: meshes) {
    mesh.vao->Bind();
    for (auto &it: mesh.matToIBO) {
      it.first->Bind();
      it.second->Bind();
      glDrawElements(GL_TRIANGLES, (int) it.second->getCount(), GL_UNSIGNED_INT, nullptr);
    }
  }
}

unordered_map<string, shared_ptr<Material>> Mesh::loadMats(const string &fileName) {
  printf("Loading materials...\n");
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
      mats[currentMat] = make_shared<Material>(*Shader::LoadShader("light"), tokens[1]);
    } else if (tokens[0] == "Ns") {
      mats[currentMat]->matData.shininess = stof(tokens[1]);
    } else if (tokens[0] == "map_Ns") {
      mats[currentMat]->shininessTex = make_shared<Texture>(tokens[1]);
    } else if (tokens[0] == "Ka") {
      mats[currentMat]->matData.ambientColor = sToVec3(tokens, 1);
    } else if (tokens[0] == "map_Ka") {
      mats[currentMat]->ambientTex = make_shared<Texture>(tokens[1]);
    } else if (tokens[0] == "Kd") {
      mats[currentMat]->matData.diffuseColor = sToVec3(tokens, 1);
    } else if (tokens[0] == "map_Kd") {
      mats[currentMat]->diffuseTex = make_shared<Texture>(tokens[1]);
    } else if (tokens[0] == "Ks") {
      mats[currentMat]->matData.specColor = sToVec3(tokens, 1);
    } else if (tokens[0] == "map_Ks") {
      mats[currentMat]->specTex = make_shared<Texture>(tokens[1]);
    } else if (tokens[0] == "Ke") {
      // TODO: Not using this yet
      printf("Don't support Ke yet in MTL\n");
    } else if (tokens[0] == "Ni") {
      mats[currentMat]->matData.refractionIndex = stof(tokens[1]);
    } else if (tokens[0] == "d") {
      mats[currentMat]->matData.alpha = stof(tokens[1]);
    } else if (tokens[0] == "illum") {
      // TODO: Not using this yet
      printf("Don't support illum yet in MTL\n");
    }
  }

  return mats;
}

bool Mesh::loadMesh(const string &fileName) {
  printf("Loading mesh...\n");
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

  // --Indexed by mesh idx--

  // Used to build vertex buffer
  vector<vector<MeshVertex>> vertices;
  // Maps position/uv/normal triplet to vertices index
  vector<unordered_map<TriIdxVals, uint32_t>> triVertexMap{};

  /* Maps mesh to material to tri array
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

        if (!triVertexMap[meshIdx].contains(idxVals)) {
          triVertexMap[meshIdx][idxVals] = (int) vertices[meshIdx].size();
          vertices[meshIdx].push_back(
                  {positions[(int) idxVals.x - 1], uvCoords[(int) idxVals.y - 1], normals[(int) idxVals.z - 1]});
        }

      }
      // manually generate a triangle-fan
      for (int x = 1; x < (faceVec.size() - 1); x++) {
        triIndices[meshIdx][currentMat].emplace_back(faceVec[0], faceVec[x], faceVec[x + 1]);
      }
    } else if (tokens[0] == "o") {
      meshIdx++;
      if (meshes.size() == 1) {
        printf("Not loading more than one mesh!\n");
        break;
      }
      meshes.push_back({});
      // Make room for next mesh data
      vertices.emplace_back();
      triVertexMap.emplace_back();
      triIndices.emplace_back();
      // No longer needed for next mesh
      positions.clear();
      uvCoords.clear();
      normals.clear();
    }
  }

  for (int n = 0; n < meshes.size(); n++) {
    auto &meshData = meshes[n];
    meshData.vao = make_unique<VertexArray>();
    VertexBuffer vbuf(&vertices[n][0], vertices[n].size() * sizeof(MeshVertex));
    BufferLayout layout;
    layout.Push<float>(3); // Pos
    layout.Push<float>(2); // UV
    layout.Push<float>(3); // Normal
    meshData.vao->AddBuffer(vbuf, layout);

    for (auto &it: meshData.matToIBO) {
      // Tris specific to this material
      auto matTris = triIndices[n][it.first];
      uint32_t numIndices = matTris.size() * 3;
      uint32_t indices[numIndices];

      // For each triangle
      for (int i = 0; i < matTris.size(); i++) {
        // 3-Tuple of pos/uv/normal idx triplet
        // (One for each vertex of triangle)
        auto t = matTris[i];
        int idx = i * 3;
        // Map pos/uv/normal idx triplet to index in vertex buffer
        indices[idx] = triVertexMap[n][get<0>(t)];
        indices[idx + 1] = triVertexMap[n][get<1>(t)];
        indices[idx + 2] = triVertexMap[n][get<2>(t)];
      }
      meshData.matToIBO[it.first] = std::make_unique<IndexBuffer>(
              &indices[0], sizeof(indices) / sizeof(uint32_t));
    }
  }

  printf("Mesh loaded!\n");
  return true;
}

#pragma clang diagnostic pop