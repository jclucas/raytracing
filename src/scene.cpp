#include <vector>
#include <ctime>
#include <iostream>
#include <glm/vec3.hpp>

#include "scene.h"
#include "kd.h"
#include "object.h"
#include "light.h"
#include "photon.h"

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

/**
 * Get scene lights.
 */
vector<Light*>& Scene::getLights() {
    return lights;
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
        glm::vec3 pos = (*i)->getPosition();
    }

}

/**
 * Create a list of renderable primitives from all scene objects.
 * @return pointer to primitives list
 */
vector<Primitive*>* Scene::getPrimitives() {

    vector<Primitive*>* prims = new vector<Primitive*>();

    // iterate through scene objects, adding primitives to list
    vector<Primitive*>* obj;
    for (auto it = objects.begin(); it != objects.end(); it++) {
        obj = (*it)->getPrimitives();
        prims->insert(prims->end(), obj->begin(), obj->end());
    }

    return prims;

}

/**
 * Create K-D tree for rendering.
 */
void Scene::generateTree(vector<Primitive*>* prims) {

    // start clock
    std::clock_t start = std::clock();

    // generate tree
    tree = new KDTree(prims);
    double duration = std::difftime(std::clock(), start) / (double) CLOCKS_PER_SEC;
    cout << "k-d tree generated after " << duration << " seconds." << endl;

}

void Scene::generatePhotonMap(int numPhotons) {

    // start clock
    std::clock_t start = std::clock();

    // get total scene intensity
    float total = 0;

    for (auto it = lights.begin(); it != lights.end(); it++) {
        total += (*it)->getIntensity();
    }

    // distribute photons by relative intensity
    int toGenerate[lights.size()] = {};
    int remaining = numPhotons;

    for (size_t i = 0; i < lights.size() - 1; i++) {
        toGenerate[i] = numPhotons * (lights[i]->getIntensity() / total);
        remaining -= toGenerate[i];
    }

    toGenerate[lights.size() - 1] = remaining;

    // generate photon list
    Photon* list = new Photon[numPhotons];
    int count = 0;

    for (size_t i = 0; i < lights.size(); i++) {
        lights[i]->spawnPhotons(toGenerate[i], &list[count]);
        count += toGenerate[i];
    }

    // TODO: not this
    vector<Photon*>* photons = new vector<Photon*>();
    BoundingBox bound = BoundingBox();

    // trace
    for (size_t i = 0; i < numPhotons; i++) {
        list[i].pos = castMonteCarlo(list[i].pos, list[i].direction, 0);
        if (list[i].pos != glm::vec3(INFINITY, INFINITY, INFINITY)) {
            photons->push_back(&list[i]);
            bound.expand(list[i].pos);
        }
    }

    // generate map
    map = new PhotonMap(photons, bound);

    // insert photons
    for (auto it = photons->begin(); it != photons->end(); it++) {
        map->insert((*it));
    }

    // report time
    double duration = std::difftime(std::clock(), start) / (double) CLOCKS_PER_SEC;
    cout << "photon map generated after " << duration << " seconds." << endl;

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
Hit Scene::cast(glm::vec3 origin, glm::vec3 direction) {
    return tree->intersect(origin, direction);
}

/**
 * Get an illuminance value by casting a ray into the scene.
 * @param origin origin of the ray
 * @param direction direction of the ray
 * @return pixel "color"
 */
glm::vec3 Scene::getPixel(glm::vec3 origin, glm::vec3 direction, int depth) {
    
    glm::vec3 color;
    Hit hit = cast(origin, direction);
    
    // direct & specular
    if (hit.object == nullptr) {
        return background;
    } else {
        color = hit.object->getColor(hit.point, origin, direction, *this, depth);
    }

    // sample photon map for global illumination
    minheap* heap = new minheap(MinSquaredDist(hit.point), vector<Photon*>());
    map->locatePhotons(hit.point, 10.0f, heap);

    if (heap->size() > 0) {
        Photon* nearest = heap->top();
        // visualize photons
        if (glm::distance(nearest->pos, hit.point) < 1.0f) {
            return glm::vec3(1);
        }
    }

    return color;

}

/**
 * @return position of photon following ray
 */
glm::vec3 Scene::castMonteCarlo(glm::vec3 origin, glm::vec3 direction, int depth) {
    
    Hit hit = cast(origin, direction);
    
    if (hit.object == nullptr) {
        return glm::vec3(INFINITY, INFINITY, INFINITY);
    } else {
        return hit.object->bounce(hit.point, direction, *this, depth);
    }

}