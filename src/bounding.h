#pragma once

#include <vector>
#include <glm/vec3.hpp>

using std::vector;

class BoundingBox {

    public:
        glm::vec3 min, max;
        BoundingBox();
        BoundingBox(glm::vec3 min, glm::vec3 max);
        BoundingBox(vector<glm::vec3>);
        BoundingBox(vector<BoundingBox> boxes);
        void expand(glm::vec3 other);
        void expand(BoundingBox& other);
        bool intersect(glm::vec3 other);
        bool intersect(BoundingBox& other);

};

class Plane {

    public:
        int axis;
        glm::vec3 normal;
        float d;
        Plane(int axis, float d);
        float intersect(glm::vec3 origin, glm::vec3 direction);

};