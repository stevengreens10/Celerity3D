#ifndef GUI_INPUT_H
#define GUI_INPUT_H

#include <unordered_map>
#include "glm/vec2.hpp"

using std::unordered_map;

class Input {
private:
    inline static unordered_map<int, bool> keysDown;
    inline static glm::vec2 mousePos;
public:

    static void HandleKeyPressEvent(int key);

    static void HandleKeyReleaseEvent(int key);

    static void HandleMousePressEvent(int button);

    static void HandleMouseReleaseEvent(int button);

    static void HandleMouseMoveEvent(int x, int y);

    static bool IsPressed(int key);

    static bool IsMousePressed(int button);

    static glm::vec2 MousePos();
};


#endif //GUI_INPUT_H
