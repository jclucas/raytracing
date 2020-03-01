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
        void expand(BoundingBox& other);

};