#include <vector>
// temp
#include <iostream>

#include "kd.h"
#include "object.h"

using std::vector;
using std::cout;
using std::endl;

KDTree::KDTree(vector<Primitive*>* list) {

    BoundingBox bound = BoundingBox();

    for (auto it = list->begin(); it != list->end(); it++) {
        bound.expand((*it)->getBounds());
    }

    // generate bvh
    root = new Node(list, bound);

    // insert primitives
    for (auto it = list->begin(); it != list->end(); it++) {
        insert(*it);
    }

}

void KDTree::insert(Primitive* obj) {
    root->insert(obj);
}

Hit KDTree::intersect(glm::vec3 origin, glm::vec3 direction) {

    // start recursive traversal
    return root->intersect(origin, direction);
}

/**
 * Recursively create a bounding hierarchy for a given set of primitives.
 */
Node::Node(vector<Primitive*>* list, BoundingBox bound) {

    // create bounding box
    this->bound = bound;
    BoundingBox centers = BoundingBox();

    for (auto it = list->begin(); it != list->end(); it++) {
        centers.expand((*it)->getPosition());
    }

    // get largest axis
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
        // node is a leaf
        contents = new vector<Primitive*>();
        return;
    }

    // partition (in half for now)
    vector<Primitive*>* front = new vector<Primitive*>();
    vector<Primitive*>* rear = new vector<Primitive*>();
    float position = centers.min[axis] + size[axis] / 2.0f;
    this->plane = new Plane(normal, position);

    // divide bounding box along plane
    glm::vec3 midmin = bound.min;
    glm::vec3 midmax = bound.max;
    midmin[axis] = position;
    midmax[axis] = position;
    BoundingBox rearBound = BoundingBox(bound.min, midmax);
    BoundingBox frontBound = BoundingBox(midmin, bound.max);
    float test = 0;

    for (auto it = list->begin(); it != list->end(); it++) {
        test = (*it)->getPosition()[axis];
        test > position ? front->push_back(*it) : rear->push_back(*it);
    }

    // create front & back nodes
    this->front = new Node(front, frontBound);
    delete front;
    this->rear = new Node(rear, rearBound);    
    delete rear;

}

bool Node::isLeaf() {
    return (front == nullptr && rear == nullptr);
}

void Node::insert(Primitive* obj) {

    // if leaf node, store
    if (isLeaf()) {
        contents->push_back(obj);
        return;
    }

    if (front != nullptr && obj->intersect(front->bound)) {
        front->insert(obj);
    }

    if (rear != nullptr && obj->intersect(rear->bound)) {
        rear->insert(obj);
    }

}

Hit Node::intersect(glm::vec3 origin, glm::vec3 direction) {
    
    // glm::vec3 direction = glm::normalize(b - a);
    
    // base case: test intersection
    if (isLeaf()) {

        float dist;
        float min = INFINITY;
        int index = -1;

        // find closest intersection
        for (size_t i = 0; i < contents->size(); i++) {
            if ((dist = (*contents)[i]->intersect(origin, direction)) < min && dist > 0) {
                min = dist;
                index = i;
            }
        }

        // create return value
        Hit hit;
        hit.object = (index >= 0)? (*contents)[index] : nullptr;
        hit.point = origin + direction * min;
        return hit;

    }

    // get axis index
    // TODO: save?
    int axis = (plane->normal.x > plane->normal.y) && (plane->normal.x > plane->normal.z) ? 0 : (plane->normal.y > plane->normal.z) ? 1 : 2;
    
    // which direction are we crossing the plane?
    float distFromPlane = origin[axis] - plane->d;
    bool originInFront = distFromPlane > 0;

    // check first side
    Hit returnVal = (originInFront) ? front->intersect(origin, direction) : rear->intersect(origin, direction);


    // find signed distances
    float a = -INFINITY;
    float b = INFINITY;

    for (int i = 0; i < 3; i++) {

        if (direction[i] >= 0) {
            a = max(a, (bound.min[i] - origin[i]) / direction[i]);
            b = min(b, (bound.max[i] - origin[i]) / direction[i]);
        } else {
            a = max(a, (bound.max[i] - origin[i]) / direction[i]);
            b = min(b, (bound.min[i] - origin[i]) / direction[i]);
        }
        
    }

    float s = (plane->d - origin[axis]) / direction[axis]; 
    
    if (s < 0 || s > b){ // || glm::abs(direction[axis]) < EPSILON) {
        // traverse near node
        return (originInFront) ? front->intersect(origin, direction) : rear->intersect(origin, direction);

    } else if (s < a) {
        // traverse far node
        return (originInFront) ? rear->intersect(origin, direction) : front->intersect(origin, direction);
    } else {
        // traverse both, near->far
        
        Hit near = (originInFront) ? front->intersect(origin, direction) : rear->intersect(origin, direction);
        if (near.object == nullptr) {
            return (originInFront) ? rear->intersect(origin, direction) : front->intersect(origin, direction);
        } else {
            return near;
        }

    }

}