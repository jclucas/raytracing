#pragma once

#include <random>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "photon.h"

class Light {

    private:
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        // TODO: make static
        std::mt19937 random = std::mt19937();
        std::uniform_real_distribution<float> dist = std::uniform_real_distribution<float>(-1.0, 1.0);
    public:
        Light(glm::vec3 position, glm::vec3 color, float intensity);
        glm::vec3 getPosition();
        glm::vec3 getRadiance(glm::vec3 point);
        float getIntensity();
        virtual void transform(glm::mat4 m);
        void spawnPhotons(int numPhotons, Photon buffer[]);

};