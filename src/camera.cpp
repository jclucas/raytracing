#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 eye, glm::vec3 up) {
    this->position = position;
    this->eye = eye;
    this->up = up;
}

glm::vec3* Camera::render(size_t height, size_t width, Scene Scene) {

    // create framebuffer
    glm::vec3 *frame = new glm::vec3[height * width];

    // draw some stuff
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            frame[i*width + j] = glm::vec3(float(i) / float(height), float(j) / float(width), 0.0f);
        }
    }

    return frame;

}