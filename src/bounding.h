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

};

class Plane {

    public:
        glm::vec3 normal;
        float d;
        Plane(glm::vec3 normal, float d);
        float intersect(glm::vec3 origin, glm::vec3 direction);

};