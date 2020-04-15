#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "bounding.h"

class Primitive;

using std::vector;

class Node {

    private:
        BoundingBox bound;
        Node *front = nullptr, *rear = nullptr;
        vector<Primitive*> contents;

    public:
        Node(vector<Primitive*>& list);
        void insert(Primitive* obj);

};

class KDTree {
    
    private:
        Node* root;

    public:
        KDTree(vector<Primitive*>& list);
        void insert(Primitive* obj);
        
};