#include "Mesh.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/hash.hpp"

Mesh::Mesh(const std::string &fileName, Material &m) : Renderable(std::reference_wrapper<Material>(m)) {
  loadMesh(fileName);
}

glm::vec3 sToVec3(std::vector<string> s, int startIdx) {
  glm::vec3 vec;
  float *f = &vec.x;
  for (int i = 0; i < 3; i++) {
    f[i] = std::stof(s[i + startIdx]);
  }
  return vec;
}

glm::vec2 sToVec2(std::vector<string> s, int startIdx) {
  glm::vec2 vec;
  float *f = &vec.x;
  for (int i = 0; i < 2; i++) {
    f[i] = std::stof(s[i + startIdx]);
  }
  return vec;
}

bool Mesh::loadMesh(const std::string &fileName) {
  printf("Loading mesh...\n");
  std::ifstream f;
  f.open(fileName);
  if (!f) {
    return false;
  }

  std::string line;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uvCoords;
  std::vector<glm::vec3> normals;
  typedef glm::vec3 TriIdxVals;
  // Used to build vertex buffer
  std::vector<MeshVertex> vertices;
  // Maps position/uv/normal triplet to vertex index
  std::unordered_map<TriIdxVals, uint32_t> triVertexMap{};
  // Used to build index buffer
  std::vector<std::tuple<TriIdxVals, TriIdxVals, TriIdxVals>> triIndices;
  int numMeshes = 0;
  while (std::getline(f, line)) {
    auto tokens = split(line, " ");
    if (tokens[0] == "v") {
      positions.push_back(sToVec3(tokens, 1));
    } else if (tokens[0] == "vt") {
      uvCoords.push_back(sToVec2(tokens, 1));
    } else if (tokens[0] == "vn") {
      normals.push_back(sToVec3(tokens, 1));
    } else if (tokens[0] == "f") {
      std::vector<TriIdxVals> faceVec;
      for (int i = 1; i < tokens.size(); i++) {
        auto faceTokens = split(tokens[i], "/");
        TriIdxVals idxVals = sToVec3(faceTokens, 0);
        faceVec.push_back(idxVals);

        if (!triVertexMap.contains(idxVals)) {
          triVertexMap[idxVals] = (int) vertices.size();
          vertices.push_back(
                  {positions[(int) idxVals.x - 1], uvCoords[(int) idxVals.y - 1], normals[(int) idxVals.z - 1]});
        }

      }
      // manually generate a triangle-fan
      for (int x = 1; x < (faceVec.size() - 1); x++) {
        triIndices.emplace_back(faceVec[0], faceVec[x], faceVec[x + 1]);
      }
    } else if (tokens[0] == "o") {
      numMeshes++;
      if (numMeshes > 1) {
        // Just quit for now
        // TODO: Add support for submeshes
        break;
      }
    }
  }

  vao = new VertexArray();
  VertexBuffer vbuf(&vertices[0], vertices.size() * sizeof(MeshVertex));
  BufferLayout layout;
  layout.Push<float>(3); // Pos
  layout.Push<float>(2); // UV
  layout.Push<float>(3); // Normal
  vao->AddBuffer(vbuf, layout);

  uint32_t numIndices = triIndices.size() * 3;
  uint32_t indices[numIndices];

  for (int i = 0; i < triIndices.size(); i++) {
    auto t = triIndices[i];
    int idx = i * 3;
    indices[idx] = triVertexMap[std::get<0>(t)];
    indices[idx + 1] = triVertexMap[std::get<1>(t)];
    indices[idx + 2] = triVertexMap[std::get<2>(t)];
  }
  ibo = new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));

  printf("Mesh loaded!\n");
  return true;
}