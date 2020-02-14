#include <fstream>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include "camera.h"

using namespace std;

int main() {

    // image size
    const int height = 800;
    const int width = 1280;

    // scene characteristics
    const glm::vec3 background = glm::vec3(0, 0.5, 1);

    // create materials
    Phong *reflective = new Phong(glm::vec3(0.8f), glm::vec3(1), 0.5f);
    Phong *transparent = new Phong(glm::vec3(0.5f), glm::vec3(0.5f), 1);
    Phong *floor = new Phong(glm::vec3(1, 0.5f, 0), glm::vec3(1), 1);

    // set up scene
    Scene scene = Scene(background);
    Triangle *tri1 = new Triangle(glm::vec3(-25, -3, 0), glm::vec3(-25, 7, 0), glm::vec3(5, -3, 0), floor);
    Triangle *tri2 = new Triangle(glm::vec3(5, 7, 0), glm::vec3(5, -3, 0), glm::vec3(-25, 7, 0), floor);
    Sphere *sphere1 = new Sphere(glm::vec3(0, 0, 2.5), 1.25f, reflective);
    Sphere *sphere2 = new Sphere(glm::vec3(-2, 1.5, 1.5), 1.0f, transparent);
    Light *light = new Light(glm::vec3(5, -1, 10), glm::vec3(1), 1);
    scene.add(*sphere1);
    scene.add(*sphere2);
    scene.add(*tri1);
    scene.add(*tri2);
    scene.add(*light);

    // set up camera
    Camera camera = Camera(glm::vec3(10, 0, 2.5), glm::vec3(-1, 0, -glm::tan(glm::radians(5.0f))), glm::vec3(0, 0, 1));

    // render
    glm::vec3 *frame = camera.render(height, width, scene);

    // save to ppm
    ofstream file;
    file.open("render.ppm");
    file << "P3 " << width << " " << height << " 255\n";

    glm::ivec3 pixel;
    for (int i = 0; i < height * width; i++) {
        pixel = glm::floor(255.0f * glm::clamp(frame[i], 0.0f, 1.0f));
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    delete[] frame;
    file.close();

}