#pragma once

#include <glm/vec3.hpp>

// maximum display luminance
#define MAX_DISP_LUM 100.0f

class ToneOperator {

    protected:
        float getLuminance(glm::vec3 tri);
        float getMaxLuminance(glm::vec3* frame, size_t size);
        float getLogAverage(glm::vec3* frame, size_t size);

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) = 0;

};

/**
 * Linear scaling from 0 to max scene value.
 */
class LinearModel : public ToneOperator {

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};

/**
 * Ward's perceptual operator.
 */
class WardModel : public ToneOperator {

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};

/**
 * Reinhard's photographic operator.
 */
class ReinhardModel : public ToneOperator {

    private:
        float gray = 0.18;

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};