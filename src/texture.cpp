#include <math.h>

#include "texture.h"

CheckTexture::CheckTexture(glm::vec3 color1, glm::vec3 color2, float uSize, float vSize) {
    this->color1 = color1;
    this->color2 = color2;
    this->uSize = uSize;
    this->vSize = vSize;
    // only diffuse texture implemented
    this->type = DIFFUSE;
}

glm::vec3 CheckTexture::getValue(float u, float v) {
    return (u * uSize - floor(u * uSize) < 0.5f) ^ (v * vSize - floor(v * vSize) < 0.5f) ? color1 : color2;
}