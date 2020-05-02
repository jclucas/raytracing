#pragma once

#include <glm/vec3.hpp>

class ToneOperator {

    protected:
        float getMaxValue(glm::vec3* frame, size_t size);
    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) = 0;

};

class LinearModel : public ToneOperator {

    private:
        float min, max;

    public:
        LinearModel();
        // LinearModel(float min, float max);
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};

class WardModel : public ToneOperator {

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};

class ReinhardModel : public ToneOperator {

    public:
        virtual glm::vec3* apply(glm::vec3* frame, size_t h, size_t w) override;

};