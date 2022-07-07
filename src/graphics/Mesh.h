#ifndef GUI_MESH_H
#define GUI_MESH_H

#include <memory>
#include "Renderable.h"

using std::string, std::unordered_map, std::unique_ptr, std::shared_ptr, std::vector, std::ifstream;
using std::reference_wrapper, std::tuple, std::make_unique, std::make_shared;

struct MeshData {
    string name;
    unique_ptr<VertexArray> vao;
    unordered_map<shared_ptr<Material>, unique_ptr<IndexBuffer>> matToIBO;
    bool shouldRender = true;
};

struct MeshVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

class Mesh : public Renderable {
private:
public:
    vector<MeshData> meshes;

    explicit Mesh(const string &fileName, Material &m);

    virtual void Draw() const override;

private:
    bool loadMesh(const string &fileName);

    static unordered_map<string, shared_ptr<Material>> loadMats(const string &fileName);

};


#endif //GUI_MESH_H
