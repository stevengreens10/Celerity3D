#ifndef GUI_VERTEXBUFFER_H
#define GUI_VERTEXBUFFER_H

class VertexBuffer {
private:
    unsigned int rendererID;
public:
    VertexBuffer(const void *data, unsigned int size);

    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};

#endif //GUI_VERTEXBUFFER_H
