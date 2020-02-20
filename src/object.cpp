#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <iostream>

#include "scene.h"
#include "object.h"
#include "material.h"
#include "light.h"

using namespace std;

// OBJECT

glm::vec3 Object::getColor(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, Scene& scene) {

    glm::vec3 color = glm::vec3(0);

    glm::vec3 n = getNormal(point);
    glm::vec3 v = glm::normalize(-direction);
    glm::vec3 s;
    glm::vec3 r;

    vector<Light*>* lights = scene.getLights();

    // iterate through lights
    for (vector<Light*>::iterator i = lights->begin(); i != lights->end(); i++) {

        glm::vec3 s = glm::normalize((*i)->getPosition() - point);

        // cast shadow vector
        if (scene.cast(point + EPSILON * s, s).object != nullptr) { 
            continue;
        }

        glm::vec3 r = glm::reflect(-s, n);
        color += material->getColor(n, s, r, v, **i);

    }
    
    return color;

}

void Object::transform(glm::mat4 m) {
    position = m * glm::vec4(position, 1);
}

// SPHERE

Sphere::Sphere(glm::vec3 position, float radius, Material *material) {
    this->position = position;
    this->radius = radius;
    this->material = material;
}

float Sphere::intersect(glm::vec3 origin, glm::vec3 direction) {
    
    glm::vec3 dist = origin - position;

    // a is always 1 for normalized vectors
    float b = 2 * glm::dot(direction, dist);
    float c = glm::dot(dist, dist) - radius * radius;

    float discriminant = b * b - 4 * c;

    // return distance if ray intersects
    return (discriminant >= 0) ? (-b - sqrt(discriminant)) / 2 : INFINITY;

}

/**
 * Get the surface normal vector at a given point.
 * @param point point on the surface
 * @return normal vector
 */
glm::vec3 Sphere::getNormal(glm::vec3 point) {
    return glm::normalize(point - position);
}

// TRIANGLE

/**
 * Construct a triangle from 3 points, specified counterclockwise.
 */
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
    if (abs(glm::dot(p, e1)) < EPSILON) {
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