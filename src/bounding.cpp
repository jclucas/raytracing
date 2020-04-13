#include <glm/common.hpp>

#include "bounding.h"

BoundingBox::BoundingBox() { 
    min = glm::vec3(INFINITY);
    max = glm::vec3(-INFINITY);
};

BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) {
    this->min = min;
    this->max = max;
}

BoundingBox::BoundingBox(vector<glm::vec3> points) {
    min = *points.begin();
    max = *points.begin();
    for (auto it = points.begin(); it != points.end(); it++) {
        min = glm::min(min, *it);
        max = glm::max(max, *it);
    }
}

BoundingBox::BoundingBox(vector<BoundingBox> boxes) {
    min = boxes.begin()->min;
    max = boxes.begin()->max;
    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        min = glm::min(min, it->min);
        max = glm::max(max, it->max);
    }
}

void BoundingBox::expand(BoundingBox& other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}