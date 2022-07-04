#ifndef GUI_INDEXBUFFER_H
#define GUI_INDEXBUFFER_H


class IndexBuffer {
private:
    unsigned int rendererID;
    unsigned int count;
public:
    IndexBuffer(const unsigned int*data, unsigned int count);

    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int getCount() const { return count; }
};


#endif //GUI_INDEXBUFFER_H
