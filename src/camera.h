#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "tone.h"

class Scene;

class Camera {

    private:
        glm::vec3 position;
        glm::mat4 m;
        float fov;
        float length;
        ToneOperator* tone = nullptr;

    public:
        Camera(glm::vec3 position, glm::vec3 eye, glm::vec3 up, ToneOperator* tone = nullptr);
        glm::vec3* render(size_t height, size_t width, Scene Scene);

};