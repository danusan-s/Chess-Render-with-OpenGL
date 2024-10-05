#include "clickable_object.h"

ClickableObject::ClickableObject(glm::vec2 posn, glm::vec2 size, unsigned int function) {
    Position = posn;
    Size = size;
    id = function;
}

ClickableObject::~ClickableObject() {}

bool ClickableObject::isClicked(float x, float y) {
    return (x < Position.x + Size.x && x > Position.x && y < Position.y + Size.y && y > Position.y);
}
