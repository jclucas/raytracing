#include <vector>
#include <ctime>
#include <iostream>
#include <glm/vec3.hpp>

#include "scene.h"
#include "kd.h"
#include "object.h"
#include "material.h"
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

    vector<Photon*>* photons = new vector<Photon*>();
    BoundingBox bound = BoundingBox();

    // trace
    for (size_t i = 0; i < numPhotons; i++) {

        Photon& p = list[i];
        
        for (size_t depth = 0; depth < MAX_DEPTH; depth++) {
        
            Hit hit = cast(p.pos, p.direction);

            if (hit.object == nullptr) {
                continue;
            } else {
                
                hit.object->bounce(p, hit.point, p.direction, *this);

                if (p.direction == glm::vec3(0)) {
                    continue;
                }

                if (depth >= 1) {
                    photons->push_back(new Photon(p));
                    bound.expand(p.pos);
                }

            }

        }

    }

    delete[] list;

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
    
    glm::vec3 color = glm::vec3(0);
    Hit hit = cast(origin, direction);
    
    // direct & specular
    if (hit.object == nullptr) {
        return background;
    } else {

        color = hit.object->getDirectIllumination(hit.point, origin, direction, *this, depth);

        // recursive call
        if (depth < MAX_DEPTH) {

            // reflection
            if (hit.object->isReflective()) {
                Ray reflect = hit.object->reflect(hit.point, direction);
                color += (hit.object->material->getProbSpecular()) * getPixel(reflect.origin, reflect.direction, depth + 1);
            }

            // transmission
            if (hit.object->isTransmissive()) {
                Ray refract = hit.object->refract(hit.point, direction);
                color += hit.object->material->getProbTransmit() * getPixel(refract.origin, refract.direction, depth + 1);

            }

        }

    }

    // sample photon map for global illumination
        minheap* heap = new minheap(MinSquaredDist(hit.point), vector<Photon*>());
        float RADIUS = 0.25f;
        map->locatePhotons(hit.point, RADIUS, heap);

        if (heap->size() > 0) {
            
            // estimate radiance
            glm::vec3 flux = glm::vec3(0);
            Photon* p;
            size_t count = 0;

            while (!heap->empty() && count < NUM_PHOTONS / 100) {
                p = heap->top();
                flux += p->power;
                heap->pop();
                count++;
            }

            glm::vec3 radiance = flux / (PI * RADIUS * RADIUS);

            // add to pixel
            color += hit.object->getDiffuse(hit.point, origin, direction, radiance);

        }

    return color;

}