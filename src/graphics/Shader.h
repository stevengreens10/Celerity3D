#ifndef GUI_SHADER_H
#define GUI_SHADER_H

#include "../util.h"
#include "GL/glew.h"
#include "glm/detail/type_mat4x4.hpp"
#include "BufferLayout.h"
#include <unordered_map>

// If matrices need to be transposed
#define TRANSPOSE false

struct GlobalUniformData {
    uint32_t id;
    uint32_t size;
    BufferLayout layout;
};

enum UniformType {
    U1f, U2f, U3f, U4f,
    U1i, U2i, U3i, U4i,
    UM3f, UM4f, UHandle
};

struct UniformMapping {
    UniformType type;
    void *data;
};

class Shader {

private:
    inline static std::unordered_map<std::string, Shader *> shaders;
    inline static std::unordered_map<std::string, GlobalUniformData> globalUniforms;
    inline static std::string SHADER_PATH = "assets/shaders/";
    std::unordered_map<std::string, int> uniformCache;
    std::string name;
public:
    unsigned int id;

    ~Shader();

    void Bind() const;

    static void Unbind();

    static Shader *LoadShader(const std::string &name);

    static Shader *CreateShader(const std::string &name);

    static Shader *CreateComputeShader(const string &name);

    static unsigned int CreateGlobalUniform(const string &name, BufferLayout layout, int idx);

    static void SetGlobalUniform(const string &name, char *data, uint32_t bufSize);

    static unsigned int CreateShaderStorageBuffer(const string &name, BufferLayout layout, int idx);

    static void SetShaderStorageBuffer(const string &name, char *data, uint32_t bufSize);

    void SetUniform(const string &uniName, UniformType type, void *data, int count = 1);


private:
    explicit Shader(const std::string &baseName, bool compute = false);

    static unsigned int CompileShader(char *source, GLenum type);

    static unsigned int CreateBuffer(GLenum type, const string &name, BufferLayout layout, int idx);

    static void SetBuffer(GLenum type, const string &name, char *data, uint32_t bufSize);

    int GetUniformLocation(const string &uniName);

};


#endif //GUI_SHADER_H
