#include <vector>
#include <glm/vec3.hpp>

#include "scene.h"

using namespace std;

/**
 * Construct an empty scene.
 * @param background default radiance value
 */
Scene::Scene(glm::vec3 background) {
    this->background = background;
    this->lights = vector<Light*>();
    this->objects = vector<Object*>();
}

/**
 * Apply a transform to all objects in the scene.
 * @param m transformation matrix
 */
void Scene::transform(glm::mat4 m) {

    for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); i++) {
        (*i)->transform(m);
    }

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
 * @return radiance value
 */
glm::vec3 Scene::cast(glm::vec3 origin, glm::vec3 direction) {
    
    for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); i++) {
        if ((*i)->intersect(origin, direction)) {
            return (*i)->getColor();
        }
    }

    return background;

}