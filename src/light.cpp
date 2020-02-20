#include "light.h"

Light::Light(glm::vec3 position, glm::vec3 color, float intensity) {
    this->position = position;
    this->color = color;
    this->intensity = intensity;
}

void Light::transform(glm::mat4 m) {
    position = m * glm::vec4(position, 1);
}

glm::vec3 Light::getPosition() {
    return position;
}

glm::vec3 Light::getRadiance() {
    return color * intensity;
}