#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
// temp
#include <iostream>

#include "scene.h"
#include "object.h"
#include "material.h"
#include "light.h"

// OBJECT

glm::vec3 Object::getPosition() {
    return position;
}

BoundingBox& Object::getBounds() {
    return bound;
}

void Object::transform(glm::mat4 m) {
    position = m * glm::vec4(position, 1);
    setBounds();
}

// PRIMITIVE

glm::vec3 Primitive::getColor(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, Scene& scene) {

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

vector<Primitive*>* Primitive::getPrimitives() {
    auto v = new vector<Primitive*>();
    v->push_back(this);
    return v;
}

// SPHERE

Sphere::Sphere(glm::vec3 position, float radius, Material *material) {
    this->position = position;
    this->radius = radius;
    this->material = material;
    setBounds();
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

/**
 * Calculate axis aligned bounding box.
 */
void Sphere::setBounds() {
    bound = BoundingBox(position - glm::vec3(radius), bound.max = position + glm::vec3(radius));
}

// TRIANGLE

/**
 * Construct a triangle from 3 points, specified counterclockwise.
 */
Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material *material) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->position = (a + b + c) / 3.0f;
    this->material = material;
    setBounds();
}

void Triangle::transform(glm::mat4 m) {
    a = m * glm::vec4(a, 1);
    b = m * glm::vec4(b, 1);
    c = m * glm::vec4(c, 1);
    position = (a + b + c) / 3.0f;
    setBounds();
    // cout << "min: (" << bound.min.x << ", " << bound.min.y << ", " << bound.min.z << ")\n";
    // cout << "max: (" << bound.max.x << ", " << bound.max.y << ", " << bound.max.z << ")" << std::endl;
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

/**
 * Calculate axis aligned bounding box.
 */
void Triangle::setBounds() {
    // bound.min.x = min(a.x, b.x, c.x);
    // bound.min.y = min(a.y, b.y, c.y);
    // bound.min.z = min(a.z, b.z, c.z);
    // bound.max.x = max(a.x, b.x, c.x);
    // bound.max.y = max(a.y, b.y, c.y);
    // bound.max.z = max(a.z, b.z, c.z);
    bound = BoundingBox({a, b, c});
}

// MESH

/**
 * Create an empty mesh.
 */
Mesh::Mesh(Material* material) {
    position = glm::vec3(0, 0, 0);
    this->material = material;
    this->components = vector<Primitive*>();
    setBounds();
}

Mesh::Mesh(string filename, Material* material) {
    // TODO: ply import
}

void Mesh::setBounds() {
    
    bound = BoundingBox();

    for (auto it = components.begin(); it != components.end(); it++) {
        bound.expand((*it)->getBounds());
    }

}

void Mesh::transform(glm::mat4 m) {

    for (auto it = components.begin(); it != components.end(); it++) {
        (*it)->transform(m);
    }

    position = m * glm::vec4(position, 1);
    setBounds(); // TODO: optimize

}

vector<Primitive*>* Mesh::getPrimitives() {
    return &components;
}

/**
 * Add a triangle to the mesh.
 * @param a first point
 * @param b second point
 * @param c third point
 */
void Mesh::add(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    
    // add component to list
    Triangle* tri = new Triangle(a, b, c, material);
    components.push_back(tri);

    // recalculate (or set) mesh position
    if (components.size() == 1) {
        position = tri->getPosition();
    } else {
        float size = (float) components.size();
        position *= (size - 1.0f) / size;
        position += tri->getPosition() / size;
    }

    // expand bounding box
    bound.expand(tri->getBounds());

}