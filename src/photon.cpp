#include "photon.h"

// temp
#include <iostream>

/**
 * Recursively create a bounding hierarchy for a given set of primitives.
 * @param list primitives to be contained in subtree
 * @param bound bounding box to be divided
 */
PhotonMap::PhotonMap(vector<Photon*>* list, BoundingBox bound) {

    // create bounding box
    this->bound = BoundingBox();

    for (auto it = list->begin(); it != list->end(); it++) {
        bound.expand((*it)->pos);
    }

    // get largest axis
    glm::vec3 size = bound.max - bound.min;
    int axis = (size.x > size.y) && (size.x > size.z) ? 0 : (size.y > size.z) ? 1 : 2;

    // recursion base case
    if (list->size() < 20 || size[axis] < EPSILON) {
        // node is a leaf
        contents = new vector<Photon*>();
        return;
    }

    // partition
    vector<Photon*>* front = new vector<Photon*>();
    vector<Photon*>* rear = new vector<Photon*>();
    float median = 0;

    for (auto it = list->begin(); it != list->end(); it++) {
        median += (*it)->pos[axis];
    }

    median /= (float) list->size();
    float position = median;
    this->plane = new Plane(axis, position);

    // divide bounding box along plane
    glm::vec3 midmin = bound.min;
    glm::vec3 midmax = bound.max;
    midmin[axis] = position;
    midmax[axis] = position;
    BoundingBox rearBound = BoundingBox(bound.min, midmax);
    BoundingBox frontBound = BoundingBox(midmin, bound.max);
    float test = 0;

    // partition list of primitives by position
    for (auto it = list->begin(); it != list->end(); it++) {
        test = (*it)->pos[axis];
        test >= position ? front->push_back(*it) : rear->push_back(*it);
    }

    // couldn't partition, this is a leaf now
    if (front->size() == 0 || rear->size() == 0) {
        contents = new vector<Photon*>();
        return;
    }

    // create front & back nodes
    // std::cout << "inserting " << front->size() << " photons front" << std::endl;
    this->front = new PhotonMap(front, frontBound);
    delete front;
    // std::cout << "inserting " << rear->size() << " photons rear" << std::endl;
    this->rear = new PhotonMap(rear, rearBound);
    delete rear;

}

/**
 * @return true if the node is a leaf node
 */
bool PhotonMap::isLeaf() {
    return (front == nullptr && rear == nullptr);
}

/**
 * Recursively insert a primitive into the subtree.
 * @param obj primitive to insert
 */
void PhotonMap::insert(Photon* obj) {

    size++;

    // if leaf node, store
    if (isLeaf()) {
        contents->push_back(obj);
        return;
    }

    if (front != nullptr && obj->pos[plane->axis] > plane->d) {
        front->insert(obj);
    } else {
        rear->insert(obj);
    }

}

void PhotonMap::locatePhotons(glm::vec3 point, float radius, minheap* heap) {

    // add photons to heap
    if (isLeaf()) {

        glm::vec3 photon;
        float dist;

        for (auto it = contents->begin(); it != contents->end(); it++) {

            photon = (*it)->pos;
            dist = photon.x - point.x + photon.y - point.y + photon.z - point.z;

            if (dist < radius) {
                heap->push(*it);
            }

        }

    } else {

        // traverse subtree
        float dist = point[plane->axis] - plane->d;

        if (dist < 0) {
            // check rear then front
            rear->locatePhotons(point, radius, heap);
            if (dist * dist < radius) {
                front->locatePhotons(point, radius, heap);
            }
        } else {
            // check front then rear
            front->locatePhotons(point, radius, heap);
            if (dist * dist < radius) {
                rear->locatePhotons(point, radius, heap);
            }

        }

    }

}


// COMPARISON FOR QUEUE

MinSquaredDist::MinSquaredDist(glm::vec3 target) {
    this->target = target;
}

bool MinSquaredDist::operator() (const Photon* lhs, const Photon* rhs) {
    return (lhs->pos.x - target.x + lhs->pos.y - target.y + lhs->pos.z - target.z) < (rhs->pos.x - target.x + rhs->pos.y - target.y + rhs->pos.z - target.z);
}