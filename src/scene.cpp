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
    std::cout << "k-d tree generated after " << duration << " seconds." << endl;

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
    int toGenerateCaustic[lights.size()] = {};
    int remaining = numPhotons;
    int remainingCaustic = numPhotons;

    for (size_t i = 0; i < lights.size() - 1; i++) {
        toGenerate[i] = numPhotons * (lights[i]->getIntensity() / total);
        remaining -= toGenerate[i];
        toGenerateCaustic[i] = numPhotons * (lights[i]->getIntensity() / total);
        remainingCaustic -= toGenerateCaustic[i];
    }

    toGenerate[lights.size() - 1] = remaining;
    toGenerateCaustic[lights.size() - 1] = remainingCaustic;

    // generate photon list
    Photon* list = new Photon[numPhotons];
    Photon* caustic = new Photon[numPhotons];
    int count = 0, countCaustic = 0;

    // environment map
    bool* env = new bool[18 * 36];
    glm::vec3 dir;
    Hit result;

    for (size_t i = 0; i < lights.size(); i++) {

        // generate env map
        for (int phi = 0; phi < 36; phi++) {
            for (int theta = 0; theta < 18; theta++) {
                dir = glm::vec3(sin(theta * 10) * cos(phi * 10), sin(theta * 10) * sin(phi * 10), cos(theta * 10));
                result = cast(lights[i]->getPosition(), dir);
                env[phi * 18 + theta] = (result.object != nullptr && (result.object->isReflective() || result.object->isTransmissive()));
            }
        }


        // spawn photons
        lights[i]->spawnPhotons(toGenerate[i], &list[count]);
        lights[i]->spawnPhotons(toGenerateCaustic[i], &caustic[countCaustic], env);
        delete[] env;
        count += toGenerate[i];
        countCaustic += toGenerateCaustic[i];

        // TODO: may not generate caustic photons

    }


    // trace
    vector<Photon*>* photons = tracePhotons(list, numPhotons);
    BoundingBox bound = BoundingBox();
    for (auto it = photons->begin(); it != photons->end(); it++) {
        bound.expand((*it)->pos);
    }

    // trace caustic photons
    vector<Photon*>* caustics = tracePhotons(caustic, numPhotons);
    BoundingBox boundC = BoundingBox();
    for (auto it = caustics->begin(); it != caustics->end(); it++) {
        boundC.expand((*it)->pos);
    }

    delete[] list;
    delete[] caustic;

    // generate map
    map = new PhotonMap(photons, bound);
    causticmap = new PhotonMap(caustics, boundC);

    // insert photons
    for (auto it = photons->begin(); it != photons->end(); it++) {
        map->insert((*it));
    }

    for (auto it = caustics->begin(); it != caustics->end(); it++) {
        causticmap->insert((*it));
    }

    // report time
    double duration = std::difftime(std::clock(), start) / (double) CLOCKS_PER_SEC;
    std::cout << "photon map generated after " << duration << " seconds." << endl;

}

vector<Photon*>* Scene::tracePhotons(Photon list[], size_t numPhotons) {

    vector<Photon*>* photons = new vector<Photon*>();

    for (size_t i = 0; i < numPhotons; i++) {

        Photon& p = list[i];
        
        for (size_t depth = 0; depth < MAX_DEPTH; depth++) {
        
            Hit hit = cast(p.pos, p.direction);

            if (hit.object == nullptr) {
                continue;
            } else {
                
                bool store = hit.object->bounce(p, hit.point, p.direction, *this);

                if (p.direction == glm::vec3(0)) {
                    continue;
                }

                if (store && depth >= 1) {
                    photons->push_back(new Photon(p));
                }

            }

        }

    }

    return photons;

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

        // color = hit.object->getDirectIllumination(hit.point, origin, direction, *this);

        // // recursive call
        // if (depth < MAX_DEPTH) {

        //     // reflection
        //     if (hit.object->isReflective()) {
        //         Ray reflect = hit.object->reflect(hit.point, direction);
        //         color += (hit.object->material->getProbSpecular()) * getPixel(reflect.origin, reflect.direction, depth + 1);
        //     }

        //     // transmission
        //     if (hit.object->isTransmissive()) {
        //         Ray refract = hit.object->refract(hit.point, direction);
        //         color += hit.object->material->getProbTransmit() * getPixel(refract.origin, refract.direction, depth + 1);

        //     }

        // }

    }
    
    // global photon map
    // color += hit.object->getDiffuse(hit.point, origin, direction, map->sample(hit.point, 0.5f));
    
    // caustic photon map
    color += hit.object->getDiffuse(hit.point, origin, direction, causticmap->sample(hit.point, 0.1f));

    return color;

}