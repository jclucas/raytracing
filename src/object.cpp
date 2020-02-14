#include <glm/geometric.hpp>
#include <iostream>

#include "object.h"

using namespace std;

// OBJECT

glm::vec3 Object::getColor(glm::vec3 point, glm::vec3 origin, glm::vec3 direction) {
    glm::vec3 n = getNormal(point);
    glm::vec3 v = glm::normalize(point - origin);
    glm::vec3 s = glm::vec3(0, 10, 0);
    glm::vec3 r = glm::reflect(s, n);
    return material->getColor(n, v, s, r);
}

void Object::transform(glm::mat4 m) {
    position = m * glm::vec4(position, 1);
    // cout << "pos: " << position.x << " " << position.y << " " << position.z << endl;
}

// SPHERE

Sphere::Sphere(glm::vec3 position, float radius, Material *material) {
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

/**
 * Get the surface normal vector at a given point.
 * @param point point on the surface
 * @return normal vector
 */
glm::vec3 Sphere::getNormal(glm::vec3 point) {
    return glm::normalize(position - point);
}

// TRIANGLE

Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material *material) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->material = material;
}

void Triangle::transform(glm::mat4 m) {
    a = m * glm::vec4(a, 1);
    b = m * glm::vec4(b, 1);
    c = m * glm::vec4(c, 1);
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

/**
 * Get the surface normal vector at a given point.
 * @param point point on the surface
 * @return normal vector
 */
glm::vec3 Triangle::getNormal(glm::vec3 point) {
    return glm::normalize(glm::cross(a - b, a - c));
}