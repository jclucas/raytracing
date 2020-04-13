#include <vector>
// temp
#include <iostream>

#include "kd.h"
#include "object.h"

using std::vector;

KDTree::KDTree(vector<Object*> list) {
    root = new Node(list);
}

Node::Node(vector<Object*> list) {

    // create bounding box
    bound = BoundingBox();

    for (auto it = list.begin(); it != list.end(); it++) {
        bound.expand((*it)->getBounds());
    }

    std::cout << "bounding box: (" << bound.min.x << ", " << bound.min.y << ", " << bound.min.z << "), (";
    std::cout << bound.max.x << ", " << bound.max.y << ", " << bound.max.z << "). \n";
    std::cout << "contains " << list.size() << " objects. " << std::endl;

    // recursion base case
    if (list.size() < 3) {
        this->contents = list;
        std::cout << "terminating." << std::endl;
        return;
    }

    // get largest axis
    glm::vec3 size = bound.max - bound.min;
    int axis = (size.x > size.y) && (size.x > size.z) ? 0 : (size.y > size.z) ? 1 : 2;

    // partition (in half for now)
    vector<Object*> front, rear;
    float mid;

    for (auto it = list.begin(); it != list.end(); it++) {
        mid += (*it)->getPosition()[axis];
    }
    
    mid /= (float) list.size();
    
    std::cout << "partitioning at " << mid << " on axis " << axis << "." << std::endl;

    for (auto it = list.begin(); it != list.end(); it++) {
        (*it)->getPosition()[axis] > mid ? front.push_back(*it) : rear.push_back(*it);
    }

    // create front & back nodes
    this->front = new Node(front);
    this->rear = new Node(rear);    

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