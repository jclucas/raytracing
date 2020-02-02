#include <glm/vec3.hpp>

#include "scene.h"

class Camera {

    private:
        glm::vec3 position;
        glm::vec3 eye;
        glm::vec3 up;

    public:
        Camera(glm::vec3 position, glm::vec3 eye, glm::vec3 up);
        glm::vec3* render(size_t height, size_t width, Scene Scene);

};