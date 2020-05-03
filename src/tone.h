#pragma once

#include <glm/vec3.hpp>

// maximum display luminance
#define MAX_DISP_LUM 100.0f

class ToneOperator {

    protected:
        float getLuminance(glm::vec3 tri);
        float getMaxValue(glm::vec3* frame, size_t size);
        float getMaxLuminance(glm::vec3* frame, size_t size);
        float getLogAverage(glm::vec3* frame, size_t size);

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) = 0;

};

class LinearModel : public ToneOperator {

    private:
        float min, max;

    public:
        LinearModel();
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};

class WardModel : public ToneOperator {

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};

class ReinhardModel : public ToneOperator {

    private:
        float gray = 0.18;

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};