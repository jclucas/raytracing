#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "bounding.h"
#include "object.h"

struct Photon {
    glm::vec3 pos;
    glm::vec3 power;
    glm::vec3 direction;
};

// TODO: generify KDTree class ?

class PhotonMap {

    private:
        Plane* plane = nullptr;
        PhotonMap *front = nullptr, *rear = nullptr;
        vector<Photon*>* contents = nullptr;
        bool isLeaf();

    public:
        BoundingBox bound;
        PhotonMap(vector<Photon*>* list, BoundingBox bound);
        void insert(Photon* obj);

};