#ifndef LINE_H
#define LINE_H

#include "constant.h"
#include <glm/glm.hpp>

class Line
{
public:
    Line(glm::vec2 p1, glm::vec2 p2);
    glm::vec2 p1() { return mP1; }
    glm::vec2 p2() { return mP2; }

    bool intersect(Line other, glm::vec2& intersectPoint);
    
private:
    glm::vec2 mP1;
    glm::vec2 mP2;    
};

#endif // LINE_H
