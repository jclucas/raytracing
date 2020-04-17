#include <vector>
// temp
#include <iostream>

#include "kd.h"
#include "object.h"

using std::vector;
using std::cout;
using std::endl;

KDTree::KDTree(vector<Primitive*>* list) {

    // generate bvh
    root = new Node(list);

    // insert primitives
    for (auto it = list->begin(); it != list->end(); it++) {
        insert(*it);
    }

}

void KDTree::insert(Primitive* obj) {
    root->insert(obj);
}

Hit KDTree::intersect(glm::vec3 origin, glm::vec3 direction) {
    return root->intersect(origin, direction);
}

/**
 * Recursively create a bounding hierarchy for a given set of primitives.
 */
Node::Node(vector<Primitive*>* list) {

    // create bounding box
    bound = BoundingBox();
    BoundingBox centers = BoundingBox();

    for (auto it = list->begin(); it != list->end(); it++) {
        bound.expand((*it)->getBounds());
        centers.expand((*it)->getPosition());
    }

    std::cout << "contains " << list->size() << " objects. " << std::endl;

    // get largest axis
    // glm::vec3 size = bound.max - bound.min;
    glm::vec3 size = centers.max - centers.min;
    int axis = (size.x > size.y) && (size.x > size.z) ? 0 : (size.y > size.z) ? 1 : 2;
    glm::vec3 normal;

    switch (axis) {
        case 0:
            normal = glm::vec3(1, 0, 0);
            break;
        case 1:
            normal = glm::vec3(0, 1, 0);
            break;
        case 2:
            normal = glm::vec3(0, 0, 1);
            break;
    }

    // recursion base case
    if (list->size() < 3 || size[axis] < EPSILON) {
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
    float position = centers.min[axis] + size[axis] / 2.0f;
    
    // TODO: remove locals ?
    this->plane = new Plane(normal, position);

    std::cout << "partitioning at " << position << " on axis " << axis << "." << std::endl;
    float test = 0;

    for (auto it = list->begin(); it != list->end(); it++) {
        test = (*it)->getPosition()[axis];
        test > position ? front->push_back(*it) : rear->push_back(*it);
    }

    // create front & back nodes
    this->front = new Node(front);
    delete front;
    this->rear = new Node(rear);    
    delete rear;

}

bool Node::isLeaf() {
    return (front == nullptr && rear == nullptr);
}

void Node::insert(Primitive* obj) {

    // if leaf node, store
    if (isLeaf()) {
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


Hit Node::intersect(glm::vec3 origin, glm::vec3 direction) {
    
    // base case: test intersection
    if (isLeaf()) {

        float dist;
        float min = INFINITY;
        int index = -1;

        // find closest intersection
        for (size_t i = 0; i < contents.size(); i++) {
            if ((dist = contents[i]->intersect(origin, direction)) < min && dist > 0) {
                min = dist;
                index = i;
            }
        }

        // create return value
        Hit hit;
        hit.object = (index >= 0)? contents[index] : nullptr;
        hit.point = origin + direction * min;
        return hit;

    }

    // recursive case: traverse nearer branch first
    float dist = plane->intersect(origin, direction); // intersect ray/plane

    // parallel: traverse only current side
    if (dist == INFINITY) {
        return front->intersect(origin, direction);
    } else if (dist == -INFINITY) {
        return rear->intersect(origin, direction);
    } else {

        Hit returnVal;

        if (dist < 0) { 
            returnVal = rear->intersect(origin, direction);
            return (returnVal.object == nullptr) ? front->intersect(origin, direction) : returnVal;
        } else {
            returnVal = front->intersect(origin, direction);
            return (returnVal.object == nullptr) ? rear->intersect(origin, direction) : returnVal;
        }

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