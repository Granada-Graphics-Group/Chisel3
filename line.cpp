#include "line.h"

Line::Line(glm::vec2 p1, glm::vec2 p2)
:
    mP1(p1),
    mP2(p2)
{
}

bool Line::intersect(Line other, glm::vec2 &p2)
{
    float a1 = mP2.y - mP1.y;
    float b1 = mP1.x - mP2.x;
    float c1 = a1 * mP1.x + b1 * mP1.y;
    
    float a2 = other.p2().y - other.p1().y;
    float b2 = other.p1().x - other.p2().x;
    float c2 = a2 * other.p1().x + b2 * other.p2().y;

    float det = a1 * b2 - a2 * b1;
    
    if(fabs(det) < EPSILON)
    {
        return false;        
    }
    else
    {
        p2.x = (b2 * c1 - b1 * c2) / det;
        p2.y = (a1 * c2 - a2 * c1) / det;
        return true;        
    }    
}
