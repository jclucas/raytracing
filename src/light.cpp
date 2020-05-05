#include <iostream>
#include <glm/trigonometric.hpp>

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
            x = 2.0 * dist(random) - 1.0;
            y = 2.0 * dist(random) - 1.0;
            z = 2.0 * dist(random) - 1.0;
        } while (x * x + y * y + z * z > 1);

        buffer[count].direction = glm::normalize(glm::vec3(x, y, z));
        // buffer[count].direction = glm::vec3(-1, -1, -1);
        buffer[count].pos = this->position;
        buffer[count].power = power;

        // uniform distribution of wavelengths 400-700nm
        buffer[count].wavelength = (300.0f * dist(random)) + 400.0f;

        count++;

    }

}

void Light::spawnPhotons(int numPhotons, Photon buffer[], const bool env[]) {

    int count = 0;
    float power = this->intensity / (float) numPhotons;
    float angle = 0.0;

    // get solid angle
    for (int i = 0; i < 18 * 36; i ++) {
        if (env[i]) angle++;
    }

    // nothing to generate
    if (angle == 0.0) return;

    angle /= (18 * 36);

    int index;
    float phi, theta, dp, dt;
    glm::vec3 dir;

    while (count < numPhotons) {

        // pick a section
        do {
            index = (int) floor(dist(random) * 18 * 36);
        } while (!env[index]);

        // pick direction within section
        dp = dist(random) * 10.0f;
        dt = dist(random) * 10.0f;
        phi = glm::radians((float) (index / 18) * 10.0 + dp);
        theta = glm::radians((float) (index % 18) * 10.0 + dt);
        
        dir = glm::vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

        buffer[count].direction = glm::normalize(dir);
        buffer[count].pos = this->position;
        buffer[count].power = power * angle;

        // uniform distribution of wavelengths 400-700nm
        buffer[count].wavelength = (300.0f * dist(random)) + 400.0f;

        count++;

    }

}