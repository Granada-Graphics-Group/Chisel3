#include "geomhelper.h"
#include <glm/gtx/rotate_vector.hpp>

bool sameSide(glm::dvec2 p1, glm::dvec2 p2, glm::dvec2 a, glm::dvec2 b)
{
//     auto cp1 = cross(b - a, p1 - a);
//     auto cp2 = cross(b - a, p2 - a);
// 
//     return glm::dot(cp1, cp2) >= 0;
    
    auto dA = (a.x - p1.x) * (p2.y - p1.y) - (a.y - p1.y) * (p2.x - p1.x);
    auto dB = (b.x - p1.x) * (p2.y - p1.y) - (b.y - p1.y) * (p2.x - p1.x);
    
    return ((dA > 0 && dB > 0) || (dA < 0 && dB < 0)) ? true : false;
    
}

glm::vec2 computePoint(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c, glm::dvec2 d, glm::dvec2 e, glm::dvec2 g)
{
    auto lengthAB = glm::length(b - a);
    auto lengthAC = glm::length(c - a);
    auto lengthDE = glm::length(e - d);
    
    auto lengthDF = (lengthDE * lengthAC) / lengthAB;
    
    auto angle = glm::acos(glm::dot(b - a, c - a) / (lengthAB * lengthAC));
    auto vecDF = glm::normalize(glm::rotate(e - d, angle));
    
    auto pointF = d + vecDF * lengthDF;
    
    if(sameSide(d, e, pointF, g))
    {
        vecDF = glm::normalize(glm::rotate(e - d, -angle));
        pointF = d + vecDF * lengthDF;
    }
    
    return pointF;
}

glm::vec2 computeNormal(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c)
{
    glm::dvec2 distance = b - a;
    glm::dvec2 normal(distance.y, -distance.x);
    
    normal = glm::normalize(normal);
    
    glm::dvec2 testPoint = a + normal * 0.2;
    
    if(sameSide(a, b, testPoint, c))
        normal = glm::normalize(glm::vec2(-distance.y, distance.x));
    
    return normal;
}

bool ptInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) 
{
    auto dX = p.x - p2.x;
    auto dY = p.y - p2.y;
    auto dX21 = p2.x - p1.x;
    auto dY12 = p1.y - p2.y;
    auto D = dY12 * (p0.x - p2.x) + dX21 * (p0.y - p2.y);
    auto s = dY12 * dX + dX21 * dY;
    auto t = (p2.y - p0.y) * dX + (p0.x - p2.x) * dY;
    if (D < 0) return s <= 0 && t <= 0 && (s + t) >= D;
    return s >= 0 && t >= 0 && s + t <= D;
}
