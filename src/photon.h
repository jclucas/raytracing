#pragma once

#include <vector>
#include <queue>
#include <glm/vec3.hpp>

#include "bounding.h"
#include "object.h"

struct Photon {
    glm::vec3 pos;
    glm::vec3 power;
    glm::vec3 direction;
};

// comparison for min heap
class MinSquaredDist {

    glm::vec3 target;

    public:
        MinSquaredDist(glm::vec3 target);
        bool operator() (const Photon* lhs, const Photon* rhs);

};

typedef struct std::priority_queue<Photon, vector<Photon*>, MinSquaredDist> minheap;

// TODO: generify KDTree class ?

class PhotonMap {

    private:
        Plane* plane = nullptr;
        PhotonMap *front = nullptr, *rear = nullptr;
        vector<Photon*>* contents = nullptr;
        bool isLeaf();

    public:
        BoundingBox bound;
        size_t size = 0;
        PhotonMap(vector<Photon*>* list, BoundingBox bound);
        void insert(Photon* obj);
        void locatePhotons(glm::vec3 point, float radius, minheap* heap);

};