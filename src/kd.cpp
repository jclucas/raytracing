#include <vector>
// temp
#include <iostream>

#include "kd.h"
#include "object.h"

using std::vector;
using std::cout;
using std::endl;

KDTree::KDTree(vector<Primitive*>& list) {

    // generate bvh
    root = new Node(list);

    // insert primitives
    for (auto it = list.begin(); it != list.end(); it++) {
        insert(*it);
    }

}

void KDTree::insert(Primitive* obj) {
    root->insert(obj);
}

/**
 * Recursively create a bounding hierarchy for a given set of primitives.
 */
Node::Node(vector<Primitive*>& list) {

    // create bounding box
    bound = BoundingBox();
    BoundingBox centers = BoundingBox();

    for (auto it = list.begin(); it != list.end(); it++) {
        bound.expand((*it)->getBounds());
        centers.expand((*it)->getPosition());
    }

    std::cout << "bounding box: (" << bound.min.x << ", " << bound.min.y << ", " << bound.min.z << "), (";
    std::cout << bound.max.x << ", " << bound.max.y << ", " << bound.max.z << "). \n";
    std::cout << "contains " << list.size() << " objects. " << std::endl;

    // get largest axis
    // glm::vec3 size = bound.max - bound.min;
    glm::vec3 size = centers.max - centers.min;
    int axis = (size.x > size.y) && (size.x > size.z) ? 0 : (size.y > size.z) ? 1 : 2;

    // recursion base case
    if (list.size() < 3 || size[axis] < EPSILON) {
        // copy list to leaf node
        // for (auto it = list.begin(); it != list.end(); it++) {
        //     contents.push_back(*it);
        // }
        std::cout << "terminating." << std::endl;
        return;
    }

    // partition (in half for now)
    vector<Primitive*>* front = new vector<Primitive*>();
    vector<Primitive*>* rear = new vector<Primitive*>();
    float mid = centers.min[axis] + size[axis] / 2.0f;
    
    std::cout << "partitioning at " << mid << " on axis " << axis << "." << std::endl;
    float test = 0;

    for (auto it = list.begin(); it != list.end(); it++) {
        test = (*it)->getPosition()[axis];
        test > mid ? front->push_back(*it) : rear->push_back(*it);
    }

    // create front & back nodes
    this->front = new Node(*front);
    delete front;
    this->rear = new Node(*rear);    
    delete rear;

}

void Node::insert(Primitive* obj) {

    // if leaf node, store
    if (front == nullptr && rear == nullptr) {
        contents.push_back(obj);
        return;
    }

    if (front != nullptr && obj->intersect(front->bound)) {
        front->insert(obj);
    }

    if (rear != nullptr && obj->intersect(rear->bound)) {
        front->insert(obj);
    }

}

/**
 * Insert a set of objects into the tree.
 */
// template <typename T>
// void KDTree<T>::insert(vector<T*> ptr) {

//     if (root == nullptr) {
//         root = new Node<T>;
//     }

// }

/**
 * Helper method:
 * Recursively insert a set of objects into the tree at a specified level.
 */
// template <typename T>
// void insert(vector<T*> ptr, Node* parent) {

//     if (parent == nullptr) {
//         parent = new Node<T>;
//     }

// }