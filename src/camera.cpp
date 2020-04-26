#include <iostream>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

#include "scene.h"
#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up) {

    this->position = position;
    
    glm::vec3 n = glm::normalize(lookat - position);
    glm::vec3 u = glm::normalize(glm::cross(up, n));
    glm::vec3 v = glm::cross(n, u);

    // camera transform matrix
    m = glm::mat4(u.x, v.x, n.x, 0,
                  u.y, v.y, n.y, 0,
                  u.z, v.z, n.z, 0,
                  -glm::dot(position, u), -glm::dot(position, v), -glm::dot(position, n), 1);

    // hardcoded parameters for now
    this->fov = glm::radians(30.0f);
    this->length = 0.035f;

}

/**
 * Render a scene.
 * @param height height of image in pixels
 * @param width width of image in pixels
 * @param scene scene to render
 */
glm::vec3* Camera::render(size_t height, size_t width, Scene scene) {

    vector<Primitive*>* prims = scene.getPrimitives();

    // transform scene to camera space    
    scene.transform(m);
    position = m * glm::vec4(position, 1);

    // create k-d tree
    scene.generateTree(prims);

    // generate photon map
    scene.generatePhotonMap(NUM_PHOTONS);

    // create framebuffer
    glm::vec3 *frame = new glm::vec3[height * width];

    // define film plane
    glm::vec3 center = glm::vec3(0, 0, length);
    // float w = glm::tan(fov) * length;
    float h = 0.025f;

    // amount to step in camera space between pixels
    float step = -h / height;
    glm::vec4 dw = glm::vec4(step, 0, 0, 0);
    glm::vec4 dh = glm::vec4(0, step, 0, 0);
    
    // upper left corner ray
    glm::vec4 ul = glm::vec4(center, 0);
    ul -= (float(width) / 2 - 0.5f) * dw;
    ul -= (float(height) / 2 - 0.5f) * dh;

    std::cout << "rendering..." << std::endl;

    glm::vec3 dir;
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            dir = glm::normalize(glm::vec3(ul + dw * float(j) + dh * float(i)));
            frame[i*width + j] = scene.getPixel(position, dir, 1);
        }
    }

    return frame;

}