#include <vector>
#include <glm/vec3.hpp>

#include "scene.h"
#include "kd.h"
#include "object.h"
#include "light.h"

// temp
#include <iostream>

// TODO: change
using namespace std;

/**
 * Construct an empty scene.
 * @param background default radiance value
 */
Scene::Scene(glm::vec3 background) {
    this->background = background;
    this->lights = vector<Light*>();
    this->objects = vector<Object*>();
    this->tree = nullptr;
}


vector<Light*>* Scene::getLights() {
    return &lights;
}

/**
 * Apply a transform to all objects in the scene.
 * @param m transformation matrix
 */
void Scene::transform(glm::mat4 m) {

    for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); i++) {
        (*i)->transform(m);
    }

    for (vector<Light*>::iterator i = lights.begin(); i != lights.end(); i++) {
        (*i)->transform(m);
    }

}

/**
 * Create K-D tree for rendering.
 */
void Scene::generateTree() {
    tree = new KDTree(objects);
}

/**
 * Add a point light source to the scene.
 * @param light the light source to add
 */ 
void Scene::add(Light &light) {
    lights.push_back(&light);
}

/**
 * Add an object to the scene.
 * @param object the object to add
 */ 
void Scene::add(Object &object) {
    objects.push_back(&object);
}

/**
 * Cast a ray into the scene.
 * @param origin origin of the ray
 * @param direction direction of the ray
 * @return pointer to intersected object or null pointer
 */
Scene::Hit Scene::cast(glm::vec3 origin, glm::vec3 direction) {
    
    float dist;
    float min = INFINITY;
    int index = -1;

    // intersect all objects
    for (int i = 0; i < objects.size(); i++) {
        if ((dist = objects[i]->intersect(origin, direction)) < min && dist > 0) {
            min = dist;
            index = i;
        }
    }

    // create return value
    Scene::Hit hit;
    hit.object = (index >= 0)? objects[index] : nullptr;
    hit.point = origin + direction * min;
    return hit;

}

glm::vec3 Scene::getPixel(glm::vec3 origin, glm::vec3 direction) {
    
    Scene::Hit hit = cast(origin, direction);
    
    if (hit.object == nullptr) {
        return background;
    } else {
        return hit.object->getColor(hit.point, origin, direction, *this);
    }

}