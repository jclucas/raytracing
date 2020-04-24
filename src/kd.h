#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "bounding.h"
#include "scene.h"

class Primitive;

using std::vector;

class Node {

    private:
        Plane* plane = nullptr;
        Node *front = nullptr, *rear = nullptr;
        vector<Primitive*>* contents = nullptr;
        bool isLeaf();

    public:
        BoundingBox bound;
        Node(vector<Primitive*>* list, BoundingBox bound);
        void insert(Primitive* obj);
        Hit intersect(glm::vec3 origin, glm::vec3 direction, float a, float b);

};

class KDTree {
    
    private:
        Node* root;

    public:
        KDTree(vector<Primitive*>* list);
        void insert(Primitive* obj);
        Hit intersect(glm::vec3 origin, glm::vec3 direction);
        
};