#ifndef GUI_MESH_H
#define GUI_MESH_H

#include <memory>
#include "Object.h"
#include <string>

using std::string, std::unordered_map, std::unique_ptr, std::shared_ptr, std::vector, std::ifstream;
using std::reference_wrapper, std::tuple, std::make_unique, std::make_shared;

struct MeshData {
    string name;
    unordered_map<shared_ptr<Material>, shared_ptr<IndexBuffer>> matToIBO;
    bool shouldRender = true;
};

class Mesh : public Object {
private:
public:
    std::unique_ptr<VertexArray> vao;
    vector<MeshData> meshes;

    explicit Mesh(const string &fileName);

    void Draw(Shader &shader) override;

private:
    bool loadMesh(const string &fileName);

    static unordered_map<string, shared_ptr<Material>> loadMats(const string &fileName);

};


#endif //GUI_MESH_H
