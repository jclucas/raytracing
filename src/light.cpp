#include <iostream>

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

float Light::getIntensity() {
    return intensity;
}


void Light::spawnPhotons(int numPhotons, Photon buffer[]) {

    int count = 0;
    float power = this->intensity / (float) numPhotons;
    float x, y, z;

    while (count < numPhotons) {

        do {
            x = dist(random);
            y = dist(random);
            z = dist(random);
        } while (x * x + y * y + z * z > 1);

        buffer[count].direction = glm::normalize(glm::vec3(x, y, z));
        buffer[count].pos = this->position;
        buffer[count].power = this->color * power;

        count++;

    }

}