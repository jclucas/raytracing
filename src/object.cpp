#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

#include "object.h"

// OBJECT

glm::vec3 Object::getColor() {
    return material;
}

// SPHERE

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 material) {
    this->position = position;
    this->orientation = glm::vec3(0, 0, 0);
    this->radius = radius;
    this->material = material;
}

bool Sphere::intersect(glm::vec3 origin, glm::vec3 direction) {
    
    glm::vec3 dist = origin - position;

    float a = glm::dot(direction, direction);
    float b = 2 * glm::dot(direction, dist);
    float c = glm::dot(dist, dist) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    return discriminant > 0;

}