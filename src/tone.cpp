#include <glm/geometric.hpp>
#include "tone.h"

float ToneOperator::getMaxValue(glm::vec3* frame, size_t size) {

    // find maximum intensity component
    float maxval = 0;
    for (size_t i = 0; i < size; i++) {
        if (frame[i].r > maxval) { maxval = frame[i].r; }
        if (frame[i].g > maxval) { maxval = frame[i].g; }
        if (frame[i].b > maxval) { maxval = frame[i].b; }
    }

    return maxval;

}

LinearModel::LinearModel() {
    this->min = 0;
    this->max = 0;
}

glm::vec3* LinearModel::apply(glm::vec3* frame, size_t h, size_t w) {

    // get max luminance component
    max = getMaxValue(frame, h * w);

    // create output buffer
    glm::vec3* output = new glm::vec3[h * w];

    // store scaled value
    for (int i = 0; i < h * w; i++) {
        output[i] = min + glm::max(glm::vec3(0), frame[i]) / (max - min);
    }

    return output;

}