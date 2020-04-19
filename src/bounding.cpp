#include <glm/common.hpp>
#include <iostream>

#include "bounding.h"
#include "object.h"

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

void BoundingBox::expand(glm::vec3 other) {
    min = glm::min(min, other);
    max = glm::max(max, other);
}

void BoundingBox::expand(BoundingBox& other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

bool BoundingBox::intersect(BoundingBox& other) {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);

}


// BOUNDING PLANE

Plane::Plane(glm::vec3 normal, float d) {
    this->normal = normal;
    this->d = d;
}

float Plane::intersect(glm::vec3 origin, glm::vec3 direction) {

    // check if parallel
    float denom = glm::dot(normal, direction);

    if (glm::abs(denom) < EPSILON) {
        return INFINITY;
    }

    return (d - glm::dot(origin, normal)) / denom;

}