#pragma once

#include <glm/vec3.hpp>

enum TextureType {
    DIFFUSE, NORMAL
};

class Texture {

    public:
        TextureType type;
        virtual glm::vec3 getValue(float u, float v) = 0;

};

class CheckTexture : public Texture {

    private:
        glm::vec3 color1, color2;
        float uSize, vSize;
    public:
        CheckTexture(glm::vec3 color1, glm::vec3 color2, float uSize, float vSize);
        virtual glm::vec3 getValue(float u, float v) override;

};