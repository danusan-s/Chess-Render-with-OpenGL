#ifndef CLICKABLEOBJECT_H
#define CLICKABLEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class ClickableObject{
    private:
        glm::vec2 Position,Size;
    public:
        ClickableObject();
        ~ClickableObject();
        bool isClicked(float x,float y);
};
#endif

