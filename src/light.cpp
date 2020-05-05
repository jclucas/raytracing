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

glm::vec3 Light::getRadiance(glm::vec3 point) {
    glm::vec3 radiance = color * intensity;
    float sqdist = glm::dot(position - point, position - point);
    return (sqdist < 1) ? radiance : radiance / sqdist;
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
        // buffer[count].direction = glm::vec3(-1, -1, -1);
        buffer[count].pos = this->position;
        buffer[count].power = power;

        // uniform distribution of wavelengths 400-700nm
        buffer[count].wavelength = (150.0f * dist(random)) + 550.0f;

        count++;

    }

}

void Light::spawnPhotons(int numPhotons, Photon buffer[], const bool env[]) {

    int count = 0;
    float power = this->intensity / (float) numPhotons;
    float x, y, z;
    int phi, theta;
    float angle = 0.0;

    // get solid angle
    for (int i = 0; i < 18 * 36; i ++) {
        if (env[i]) angle++;
    }

    // nothing to generate
    if (angle == 0.0) return;

    angle /= (18 * 36);
    std::cout << angle << std::endl;

    while (count < numPhotons) {

        do {
            x = dist(random);
            y = dist(random);
            z = dist(random);
            phi = (int) floor(atan(y / x) / 10);
            theta = (int) floor(acos(z) / 10);
        } while ((x * x + y * y + z * z > 1) && env[phi * 18 + theta]);

        buffer[count].direction = glm::normalize(glm::vec3(x, y, z));
        buffer[count].pos = this->position;
        buffer[count].power = power / angle;

        // uniform distribution of wavelengths 400-700nm
        buffer[count].wavelength = (150.0f * dist(random)) + 550.0f;

        count++;

    }

}