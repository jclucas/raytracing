#include <vector>
#include <ctime>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>

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

void Scene::generatePhotonMap() {

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
    int remainingG = NUM_PHOTONS;
    int remainingC = NUM_PHOTONS_CAUSTIC;

    for (size_t i = 0; i < lights.size() - 1; i++) {
        toGenerate[i] = NUM_PHOTONS * (lights[i]->getIntensity() / total);
        remainingG -= toGenerate[i];
        toGenerateCaustic[i] = NUM_PHOTONS_CAUSTIC * (lights[i]->getIntensity() / total);
        remainingC -= toGenerateCaustic[i];
    }

    toGenerate[lights.size() - 1] = remainingG;
    toGenerateCaustic[lights.size() - 1] = remainingC;

    // generate photon list
    Photon* list = new Photon[NUM_PHOTONS];
    Photon* caustic = new Photon[NUM_PHOTONS_CAUSTIC];
    int count = 0, countCaustic = 0;

    // environment map
    bool* env = new bool[180 * 360];
    glm::vec3 dir;
    Hit result;

    for (size_t i = 0; i < lights.size(); i++) {

        // generate env map
        for (int phi = 0; phi < 360; phi++) {

            for (int theta = 0; theta < 180; theta++) {
                float radT = glm::radians((float) theta);
                float radP = glm::radians((float) phi);
                dir = glm::vec3(sin(radT) * cos(radP), sin(radT) * sin(radP), cos(radT));
                result = cast(lights[i]->getPosition(), dir);
                env[phi * 180 + theta] = (result.object != nullptr && (result.object->isReflective() || result.object->isTransmissive()));
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
    vector<Photon*>* photons = tracePhotons(list, NUM_PHOTONS);
    BoundingBox bound = BoundingBox();
    for (auto it = photons->begin(); it != photons->end(); it++) {
        bound.expand((*it)->pos);
    }

    // trace caustic photons
    vector<Photon*>* caustics = tracePhotons(caustic, NUM_PHOTONS_CAUSTIC, false);
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

vector<Photon*>* Scene::tracePhotons(Photon list[], size_t numPhotons, bool reflectDiffuse) {

    vector<Photon*>* photons = new vector<Photon*>();

    for (size_t i = 0; i < numPhotons; i++) {

        Photon& p = list[i];
        
        for (size_t depth = 0; depth < MAX_DEPTH; depth++) {
        
            Hit hit = cast(p.pos, p.direction);

            if (hit.object == nullptr) {
                break;
            } else {
                
                bool store = hit.object->bounce(p, hit.point, p.direction, *this);

                if (p.direction == glm::vec3(0)) {
                    break;
                }

                if (store) {
                    photons->push_back(new Photon(p));
                    if (!reflectDiffuse) break;
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

        color = hit.object->getDirectIllumination(hit.point, origin, direction, *this);

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
    
    // global photon map
    // color += hit.object->getDiffuse(hit.point, origin, direction, map->sample(hit.point, 0.5f, SAMPLE_SIZE));
    color += hit.object->getDiffuse(hit.point, origin, hit.object->getNormal(hit.point), map->sample(hit.point, 0.5f, 2 * SAMPLE_SIZE));
    // color += map->sample(hit.point, 0.50f, SAMPLE_SIZE);

    // caustic photon map
    // color += hit.object->getDiffuse(hit.point, origin, -hit.object->getNormal(hit.point), causticmap->sample(hit.point, 0.1f, SAMPLE_SIZE_CAUSTIC));
    color += causticmap->sample(hit.point, 0.25f, 2 * SAMPLE_SIZE_CAUSTIC);

    return color;

}