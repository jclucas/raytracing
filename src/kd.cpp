#include <vector>

#include "kd.h"
#include "object.h"

using std::vector;

/**
 * Create a BVH of a set of primitives.
 * @param list primitives to be contained in tree
 */
KDTree::KDTree(vector<Primitive*>* list) {

    // calculate initial bounding box
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

/**
 * Insert a primitive into the tree.
 * @param obj primitive to insert
 */
void KDTree::insert(Primitive* obj) {
    root->insert(obj);
}


/**
 * Perform an intersection test on the tree.
 * @param origin ray origin
 * @param direction ray direction
 * @return nearest intersection along ray
 */
Hit KDTree::intersect(glm::vec3 origin, glm::vec3 direction) {

    // find signed distances
    float a = -INFINITY;
    float b = INFINITY;

    // for each axis
    for (int i = 0; i < 3; i++) {

        if (direction[i] >= 0) {
            a = max(a, (root->bound.min[i] - origin[i]) / direction[i]);
            b = min(b, (root->bound.max[i] - origin[i]) / direction[i]);
        } else {
            a = max(a, (root->bound.max[i] - origin[i]) / direction[i]);
            b = min(b, (root->bound.min[i] - origin[i]) / direction[i]);
        }
        
    }

    // start recursive traversal
    return root->intersect(origin, direction, a, b);
}

/**
 * Recursively create a bounding hierarchy for a given set of primitives.
 * @param list primitives to be contained in subtree
 * @param bound bounding box to be divided
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

    // partition list of primitives by position
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

/**
 * @return true if the node is a leaf node
 */
bool Node::isLeaf() {
    return (front == nullptr && rear == nullptr);
}

/**
 * Recursively insert a primitive into the subtree.
 * @param obj primitive to insert
 */
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

/**
 * Recursively perform an intersection test on the subtree.
 * @param origin ray origin
 * @param direction ray direction
 * @param a signed distance along ray of first intersection
 * @param b signed distance along ray of second intersection
 * @return nearest intersection along ray in subtree
 */
Hit Node::intersect(glm::vec3 origin, glm::vec3 direction, float a, float b) {
    
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
    bool originInFront = origin[axis] > plane->d;
    float s = (plane->d - origin[axis]) / direction[axis]; 
    
    if (s < 0 || s > b || glm::abs(direction[axis]) < EPSILON) {
        // traverse near node
        return (originInFront) ? front->intersect(origin, direction, a, b) : rear->intersect(origin, direction, a, b);
    } else if (s < a) {
        // traverse far node
        return (originInFront) ? rear->intersect(origin, direction, a, b) : front->intersect(origin, direction, a, b);
    } else {
        
        // traverse both, near->far
        Hit near = (originInFront) ? front->intersect(origin, direction, a, s) : rear->intersect(origin, direction, a, s);
        
        if (near.object == nullptr) {
            return (originInFront) ? rear->intersect(origin, direction, s, b) : front->intersect(origin, direction, s, b);
        } else {
            return near;
        }

    }

}