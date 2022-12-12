#ifndef GUI_MOVECOMPONENT_H
#define GUI_MOVECOMPONENT_H

#include "../Component.h"

class MoveComponent : public Component {
private:
    float timer = 0.0f;
public:
    void Start() override;

    void Update() override;
};

#endif //GUI_MOVECOMPONENT_H
