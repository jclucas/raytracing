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
    // cout << "pos: " << position.x << " " << position.y << " " << position.z << endl;
}

// SPHERE

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 material) {
    this->position = position;
    this->radius = radius;
    this->material = material;
}

float Sphere::intersect(glm::vec3 origin, glm::vec3 direction) {
    
    glm::vec3 dist = origin - position;

    float a = glm::dot(direction, direction);
    float b = 2 * glm::dot(direction, dist);
    float c = glm::dot(dist, dist) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    // return distance if ray intersects
    return (discriminant > 0) ? (sqrt(discriminant) - b) / (2 * a) : INFINITY;

}

// TRIANGLE

Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 material) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->material = material;
}

float Triangle::intersect(glm::vec3 origin, glm::vec3 direction) {

    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - a;
    glm::vec3 t = origin - a;
    glm::vec3 p = glm::cross(direction, e2);
    glm::vec3 q = glm::cross(t, e1);

    // parallel
    if (abs(glm::dot(p, e1)) < 0.005f) {
        return false;
    }

    // distance, u, v
    glm::vec3 intersect = glm::vec3(glm::dot(q, e2), glm::dot(p, t), glm::dot(q, direction)) / glm::dot(p, e1);

    if (intersect.x >= 0 && intersect.y + intersect.z < 1 && intersect.y >= 0 && intersect.z >= 0) {
        return intersect.x;
    } else {
        return INFINITY;
    }

}

void Triangle::transform(glm::mat4 m) {
    a = m * glm::vec4(a, 1);
    b = m * glm::vec4(b, 1);
    c = m * glm::vec4(c, 1);
}