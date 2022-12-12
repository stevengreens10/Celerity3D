#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include "graphics/object/Object.h"

class Object;

class Component {
public:
    Object *object = nullptr;

    virtual void Start() = 0;

    virtual void Update() = 0;
};


#endif //GUI_COMPONENT_H
