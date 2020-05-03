#include <glm/geometric.hpp>
#include "tone.h"

/**
 * Get photometric luminance from an RGB value.
 * @param tri RGB triple
 * @return photometric value
 */
float ToneOperator::getLuminance(glm::vec3 tri) {
    return glm::dot(tri, glm::vec3(0.27f, 0.67f, 0.06f));
}

/**
 * Get the maximum photometric luminance in an image.
 * @param frame a frame buffer
 * @param size array size of buffer
 * @return photometric value
 */
float ToneOperator::getMaxLuminance(glm::vec3* frame, size_t size) {

    // find maximum intensity component
    float maxval = 0;
    float lum;

    for (size_t i = 0; i < size; i++) {
        lum = getLuminance(frame[i]);
        if (lum > maxval) { maxval = lum; }
    }

    return maxval;

}

/**
 * Get the log average luminance in a scene.
 * @param frame a frame buffer
 * @param size array size of buffer
 * @return log average
 */
float ToneOperator::getLogAverage(glm::vec3* frame, size_t size) {

    // find maximum intensity component
    float sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += glm::log(0.001f + getLuminance(frame[i]));
        
    }

    return glm::exp(sum / (float) size);

}

/**
 * Apply a simple linear scaling from 0 to the scene's max value.
 * @param frame an image
 * @param h height
 * @param w width
 * @return a tone mapped image
 */
glm::vec3* LinearModel::apply(glm::vec3* frame, size_t h, size_t w) {

    // get max luminance component
    float max = getMaxLuminance(frame, h * w);

    // create output buffer
    glm::vec3* output = new glm::vec3[h * w];

    // store scaled value
    for (int i = 0; i < h * w; i++) {
        output[i] = frame[i] / max * MAX_DISP_LUM;
    }

    return output;

}

/**
 * Apply Ward's perceptual tone reproduction operator.
 * @param frame an image
 * @param h height
 * @param w width
 * @return a tone mapped image
 */
glm::vec3* WardModel::apply(glm::vec3* frame, size_t h, size_t w) {

    // get max luminance
    float logavg = getLogAverage(frame, h * w);

    // ward scale factor
    float sf = pow((1.219f + pow(MAX_DISP_LUM / 2.0f, 0.4f) / 1.219f + pow(logavg, 0.4f)), 2.5f);

    // create output buffer
    glm::vec3* output = new glm::vec3[h * w];

    // store scaled value
    for (int i = 0; i < h * w; i++) {
        for (int axis = 0; axis < 3; axis++) {
            output[i][axis] = frame[i][axis] * sf;
        }
    }

    return output;

}

/**
 * Apply Reinhard's photographic tone reproduction operator.
 * @param frame an image
 * @param h height
 * @param w width
 * @return a tone mapped image
 */
glm::vec3* ReinhardModel::apply(glm::vec3* frame, size_t h, size_t w) {

    // get max luminance
    float logavg = getLogAverage(frame, h * w);

    // reinhard scale factor
    float sf = gray / logavg;

    // create output buffer
    glm::vec3* output = new glm::vec3[h * w];

    // store scaled value
    for (int i = 0; i < h * w; i++) {
        for (int axis = 0; axis < 3; axis++) {
            output[i][axis] = (frame[i][axis] * sf) / (1.0f + (frame[i][axis] * sf)) * MAX_DISP_LUM;
        }
    }

    return output;

}