#ifndef GUI_MESH_H
#define GUI_MESH_H

#include <memory>
#include "Renderable.h"

struct SubMesh {
    std::unique_ptr<VertexArray> vao;
    std::unique_ptr<IndexBuffer> ibo;
    std::shared_ptr<Material> m;
};

struct MeshVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

class Mesh : public Renderable {
private:
    std::vector<SubMesh> meshes;
public:
    explicit Mesh(const std::string &fileName, Material &m);

private:
    bool loadMesh(const std::string &fileName);

};


#endif //GUI_MESH_H
