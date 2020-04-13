#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "bounding.h"

class Object;

using std::vector;

class Node {

    private:
        BoundingBox bound;
        Node *front, *rear;
        vector<Object*> contents;

    public:
        Node(vector<Object*> list);

};

class KDTree {
    
    private:
        Node* root;

    public:
        KDTree(vector<Object*> list);
        
};