#include <vector>
#include <glm/vec3.hpp>

#include "scene.h"

using namespace std;

Scene::Scene(glm::vec3 background) {
    this->background = background;
    this->objects = vector<Sphere>();
}

void Scene::add(Sphere object) {
    objects.push_back(object);
}

glm::vec3 Scene::cast(glm::vec3 origin, glm::vec3 direction) {
    
    for (vector<Sphere>::iterator i = objects.begin(); i != objects.end(); i++) {
        if (i->intersect(origin, direction)) {
            return i->getColor();
        }
    }

    return background;

}