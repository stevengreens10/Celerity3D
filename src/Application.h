#ifndef GUI_APPLICATION_H
#define GUI_APPLICATION_H

#include "window.h"
#include "graphics/Renderer.h"
#include "graphics/Framebuffer.h"

class Application {
public:
    inline static Window *window;
    inline static Renderer *renderer;
    // TODO: Figure out better place
    inline static std::vector<Framebuffer *> frameBuf;
    inline static bool running = true;

    static void Init();

    static void Update();

private:

};


#endif //GUI_APPLICATION_H
