#ifndef GEOMHELPER_H
#define GEOMHELPER_H

#include <glm/glm.hpp>

inline double cross(glm::dvec2 const& a, glm::dvec2 const& b)
{
    return a.x * b.y - b.x * a.y;
}

bool sameSide(glm::dvec2 p1, glm::dvec2 p2, glm::dvec2 a, glm::dvec2 b);
glm::vec2 computePoint(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c, glm::dvec2 d, glm::dvec2 e, glm::dvec2 g);
glm::vec2 computeNormal(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c);
bool ptInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

#endif
