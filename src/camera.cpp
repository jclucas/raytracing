#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "camera.h"

using namespace std;

Camera::Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up) {
    this->position = position;
    this->lookat = lookat;
    this->up = up;
    // hardcoded parameters for now
    this->fov = glm::radians(30.0f);
    this->length = 1;
}

/**
 * Render a scene.
 * @param height height of image in pixels
 * @param width width of image in pixels
 * @param scene scene to render
 */
glm::vec3* Camera::render(size_t height, size_t width, Scene scene) {

    glm::vec3 n = glm::normalize(lookat - position);
    glm::vec3 u = glm::normalize(glm::cross(up, n));
    glm::vec3 v = glm::cross(n, u);

    // camera transform matrix
    glm::mat4 m = glm::mat4(u.x, v.x, n.x, 0,
                            u.y, v.y, n.y, 0,
                            u.z, v.z, n.z, 0,
                            -glm::dot(position, u), -glm::dot(position, v), -glm::dot(position, n), 1);

    // create framebuffer
    glm::vec3 *frame = new glm::vec3[height * width];

    // define film plane
    glm::vec3 center = position + glm::vec3(0, 0, length);
    float aspect = (float) height / width;
    float w = glm::tan(fov) * 2 * length;
    float h = w * aspect;

    // amount to step in camera space between pixels
    float step = w / width;
    glm::vec4 dw = glm::vec4(step, 0, 0, 0);
    glm::vec4 dh = glm::vec4(0, step, 0, 0);

    // transform to world space
    glm::mat4 inv = glm::inverse(m);
    glm::vec3 worldpos = inv * glm::vec4(position, 0);
    center = inv * glm::vec4(center, 0);
    dw = inv * dw;
    dh = inv * dh;
    
    // upper left corner ray
    glm::vec4 ul = glm::vec4(center, 0);
    ul -= float(width) / 2 * dw;
    ul -= float(height) / 2 * dh;

    // cout << "ul: " << ul.x << " " << ul.y << " " << ul.z <<endl;
    // cout << "dw: " << dw.x << " " << dw.y << " " << dw.z <<endl;
    // cout << "dh: " << dh.x << " " << dh.y << " " << dh.z <<endl;

    glm::vec3 dir;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            dir = glm::normalize(glm::vec3(ul + dw * float(j) + dh * float(i)) - worldpos);
            frame[i*width + j] = scene.cast(position, dir);
        }
    }

    return frame;

}