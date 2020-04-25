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
    this->bound = bound;

    // get largest axis
    glm::vec3 size = bound.max - bound.min;
    int axis = (size.x > size.y) && (size.x > size.z) ? 0 : (size.y > size.z) ? 1 : 2;

    // recursion base case
    if (list->size() < 3 || size[axis] < EPSILON) {
        // node is a leaf
        contents = new vector<Photon*>();
        return;
    }

    // partition (in half for now)
    vector<Photon*>* front = new vector<Photon*>();
    vector<Photon*>* rear = new vector<Photon*>();
    float position = bound.min[axis] + size[axis] / 2.0f;
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
        test > position ? front->push_back(*it) : rear->push_back(*it);
    }

    // create front & back nodes
    this->front = new PhotonMap(front, frontBound);
    std::cout << "inserting " << front->size() << " photons front" << std::endl;
    delete front;
    this->rear = new PhotonMap(rear, rearBound);    
    std::cout << "inserting " << rear->size() << " photons rear" << std::endl;
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

    // if leaf node, store
    if (isLeaf()) {
        contents->push_back(obj);
        return;
    }

    if (front != nullptr && front->bound.intersect(obj->pos)) {
        front->insert(obj);
    }

    if (rear != nullptr && rear->bound.intersect(obj->pos)) {
        rear->insert(obj);
    }

}