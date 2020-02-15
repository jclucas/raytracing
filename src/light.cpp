#include "light.h"

Light::Light(glm::vec3 position, glm::vec3 color, float intensity) {
    this->position = position;
    this->color = color;
    this->intensity = intensity;
}

glm::vec3 Light::getPosition() {
    return position;
}

glm::vec3 Light::getRadiance() {
    return color * intensity;
}