#ifndef CLICKABLEOBJECT_H
#define CLICKABLEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class ClickableObject{
    public:
        ClickableObject(glm::vec2 posn,glm::vec2 size,unsigned int function);
        ~ClickableObject();
        glm::vec2 Position,Size;
        unsigned int id;
        bool isClicked(float x,float y);
};
#endif

