#include <glm/geometric.hpp>
#include <iostream>

#include "object.h"

using namespace std;

// OBJECT

glm::vec3 Object::getColor() {
    return material;
}

void Object::transform(glm::mat4 m) {
    position = m * glm::vec4(position, 1);
    orientation = m * glm::vec4(orientation, 1);
    // cout << "pos: " << position.x << " " << position.y << " " << position.z << endl;
}

// SPHERE

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 material) {
    this->position = position;
    this->orientation = glm::vec3(0, 0, 0);
    this->radius = radius;
    this->material = material;
}

void Sphere::transform(glm::mat4 m) {
    Object::transform(m);
}

bool Sphere::intersect(glm::vec3 origin, glm::vec3 direction) {
    
    glm::vec3 dist = origin - position;

    float a = glm::dot(direction, direction);
    float b = 2 * glm::dot(direction, dist);
    float c = glm::dot(dist, dist) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    return discriminant > 0;

}

// PLANE

Plane::Plane(glm::vec3 position, float x, float y, glm::vec3 normal, glm::vec3 material) {
    this->position = position;
    this->orientation = normal;
    this->x = x;
    this->y = y;
    this->material = material;
}

bool Plane::intersect(glm::vec3 origin, glm::vec3 direction) {
    return true;
}