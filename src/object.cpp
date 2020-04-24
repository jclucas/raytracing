#include <iostream>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <miniply.h>

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

glm::vec3 Object::inverseTransform(glm::vec3 p) {
    return invWorldMatrix * glm::vec4(p, 1);
}

void Object::transform(glm::mat4 m) {
    position = m * glm::vec4(position, 1);
    invWorldMatrix =  invWorldMatrix * glm::inverse(m);
    setBounds();
}

// PRIMITIVE

glm::vec3 Primitive::getColor(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, Scene& scene, int depth) {

    glm::vec3 color = glm::vec3(0);

    glm::vec3 n = getNormal(point);
    glm::vec3 v = glm::normalize(-direction);
    glm::vec3 s;
    glm::vec3 r;
    glm::vec3 objPoint = inverseTransform(point);

    vector<Light*> lights = scene.getLights();

    // iterate through lights
    for (vector<Light*>::iterator i = lights.begin(); i != lights.end(); i++) {

        glm::vec3 s = glm::normalize((*i)->getPosition() - point);

        // cast shadow vector
        Hit shadow = scene.cast(point + EPSILON * n, s);
        float dist = glm::length((*i)->getPosition() - point);

        // TODO: encapsulation
        if (shadow.object != nullptr && shadow.object->material->getTransmittance() < 1.0f) {
            glm::vec3 r = glm::reflect(-s, n);
            color += shadow.object->material->getTransmittance() * material->getColor(objPoint, n, s, r, v, **i);
        } else if (shadow.object == nullptr || dist < glm::length (shadow.point - point)) { 
            glm::vec3 r = glm::reflect(-s, n);
            color += material->getColor(objPoint, n, s, r, v, **i);
        }

        // recursive call
        if (depth < MAX_DEPTH) {

            // reflection
            if (material->getReflectance() > 0) {
                glm::vec3 reflect = glm::reflect(-v, n);
                color += material->getReflectance() * scene.getPixel(point + EPSILON * n, reflect, depth + 1);
            }

            // transmission
            if (material->getTransmittance() > 0) {

                glm::vec3 refract;
                glm::vec3 norm = n;
                float dot = glm::dot(-v, n);
                float ratio = 1.0f / material->getIOR();
                
                // check if we are entering or exiting material
                if (dot > 0) {
                    norm = -norm;
                    ratio = 1.0f / ratio;
                    dot = glm::dot(-v, norm);
                }

                float sqrt = 1.0f - ratio * ratio * (1.0f - dot * dot);

                // check for total internal reflection
                if (sqrt <= 0) {
                    refract = glm::reflect(-v, norm);
                } else {
                    refract = ratio * (-v) - (ratio * dot + std::sqrt(sqrt)) * norm;
                }

                color += material->getTransmittance() * scene.getPixel(point + EPSILON * -norm, refract, depth + 1);

            }

        }

    }
    
    return color;

}

vector<Primitive*>* Primitive::getPrimitives() {
    // TODO: memory management
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

    // if negative discriminant, no intersection
    if (discriminant < 0) {
        return INFINITY;
    }

    // get zeros
    float first = (-b - sqrt(discriminant)) / 2;
    float second = (-b + sqrt(discriminant)) / 2;

    // return nearest intersection > 0
    return (first > EPSILON) ? first : (second > EPSILON) ? second : INFINITY;

}

bool Sphere::intersect(BoundingBox& bounds) {
    // use aabb intersection
    return this->bound.intersect(bounds);
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
    invWorldMatrix =  invWorldMatrix * glm::inverse(m);
    setBounds();
}

float Triangle::intersect(glm::vec3 origin, glm::vec3 direction) {

    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - a;
    glm::vec3 t = origin - a;
    glm::vec3 p = glm::cross(direction, e2);
    glm::vec3 q = glm::cross(t, e1);

    // parallel
    if (abs(glm::dot(p, e1)) < EPSILON) {
        return INFINITY;
    }

    // distance, u, v
    glm::vec3 intersect = glm::vec3(glm::dot(q, e2), glm::dot(p, t), glm::dot(q, direction)) / glm::dot(p, e1);

    if (intersect.x >= 0 && intersect.y + intersect.z < 1 && intersect.y >= 0 && intersect.z >= 0) {
        return intersect.x;
    } else {
        return INFINITY;
    }

}

bool Triangle::intersect(BoundingBox& bounds) {
    // use aabb intersection
    return this->bound.intersect(bounds);
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
    bound = BoundingBox({a, b, c});
}

// MESH

/**
 * Create an empty mesh.
 */
Mesh::Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Material* material) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    this->material = material;
    this->components = vector<Primitive*>();
    setBounds();
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
    rotation = m * glm::vec4(rotation, 1);
    invWorldMatrix =  invWorldMatrix * glm::inverse(m);
    setBounds(); // TODO: optimize

}

vector<Primitive*>* Mesh::getPrimitives() {

    glm::mat4 m = getObjectTransform();
    
    for (auto it = components.begin(); it != components.end(); it++) {
        (*it)->transform(m);
    }

    return &components;

}

/**
 * Returns a transform matrix based on the current position and rotation.
 */
glm::mat4 Mesh::getObjectTransform() {

    glm::mat4 trans = glm::mat4(1);
    trans = glm::translate(trans, position);
    trans = glm::rotate(trans, rotation.x, glm::vec3(1, 0, 0));
    trans = glm::rotate(trans, rotation.y, glm::vec3(0, 1, 0));
    trans = glm::rotate(trans, rotation.z, glm::vec3(0, 0, 1));
    trans = glm::scale(trans, scale);

    return trans;

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

    // expand bounding box
    bound.expand(tri->getBounds());
    
}

/**
 * Read triangles from a PLY file into a mesh.
 */
void Mesh::read(std::string filename) {

    miniply::PLYReader reader = miniply::PLYReader(filename.c_str());
    
    if (!reader.valid()) {
        std::cout << "Invalid file: " << filename << endl;
        exit(0);
    }

    size_t numVertices;
    size_t numTriangles;

    // create temp buffers
    float* vertices;
    uint32_t* triangles;
    uint32_t* vertexProps = new uint32_t[3];
    uint32_t* triProps = new uint32_t[3];

    // assume polygons are triangles
    miniply::PLYElement* faceElem = reader.get_element(reader.find_element(miniply::kPLYFaceElement));
    faceElem->convert_list_to_fixed_size(faceElem->find_property("vertex_indices"), 3, triProps);

    // get vertices
    if (reader.has_element() && reader.element_is(miniply::kPLYVertexElement)) {
        reader.load_element();
        reader.find_pos(vertexProps);
        numVertices = reader.num_rows();
        vertices = new float[numVertices * 3];
        reader.extract_properties(vertexProps, 3, miniply::PLYPropertyType::Float, vertices);
        reader.next_element();
    }

    if (vertices == nullptr) {
        std::cout << "failed to read vertices from " << filename << "." << endl;
        exit(0);
    }

    // get triangles
    if (reader.has_element() && reader.element_is(miniply::kPLYFaceElement)) {
        reader.load_element();
        numTriangles = reader.num_rows();
        triangles = new uint32_t[numTriangles * 3];
        reader.extract_properties(triProps, 3, miniply::PLYPropertyType::Int, triangles);
    }

    // add triangles to mesh
    int aIdx, bIdx, cIdx;
    for (size_t i = 0; i < numTriangles; i++) {
        aIdx = triangles[3 * i];
        bIdx = triangles[3 * i + 1];
        cIdx = triangles[3 * i + 2];
        add(glm::vec3(vertices[3 * aIdx], vertices[3 * aIdx + 1], vertices[3 * aIdx + 2]),
            glm::vec3(vertices[3 * bIdx], vertices[3 * bIdx + 1], vertices[3 * bIdx + 2]),
            glm::vec3(vertices[3 * cIdx], vertices[3 * cIdx + 1], vertices[3 * cIdx + 2]));
    }

    delete[] vertices;
    delete[] triangles;
    delete[] vertexProps;
    delete[] triProps;

    std::cout << "read data from " << filename << "." << endl;

}