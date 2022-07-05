#ifndef GUI_VERTEXBUFFER_H
#define GUI_VERTEXBUFFER_H

class VertexBuffer {
private:
    unsigned int rendererID;
public:
    VertexBuffer(const void *data, unsigned int size);

    void Bind() const;

    void Unbind() const;

    [[nodiscard]] inline unsigned int GetId() const { return rendererID; }
};

#endif //GUI_VERTEXBUFFER_H
