#include <math.h>

#include "texture.h"

CheckTexture::CheckTexture(glm::vec3 color1, glm::vec3 color2, float size) {
    this->color1 = color1;
    this->color2 = color2;
    this->size = size;
    // only diffuse texture implemented
    this->type = DIFFUSE;
}

glm::vec3 CheckTexture::getValue(float u, float v) {
    return (u / size - floor(u / size) < 0.5f) ^ (v / size - floor(v / size) < 0.5f) ? color1 : color2;
}