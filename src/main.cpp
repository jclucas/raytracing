#include <fstream>
//temp
#include <iostream>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include "scene.h"
#include "camera.h"
#include "object.h"
#include "material.h"
#include "texture.h"
#include "light.h"

using namespace std;

int main() {

    // output properties
    const int HEIGHT = 800;
    const int WIDTH = 1280;
    const std::string FILENAME = "render.ppm";

    // scene characteristics
    const glm::vec3 BACKGROUND = glm::vec3(0, 0.5, 1);

    // create textures
    CheckTexture *check = new CheckTexture(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), 6.0f, 3.0f);

    // create materials
    Phong *reflective = new Phong(glm::vec3(0.8f), glm::vec3(1), 10.0f);
    reflective->setReflectance(0.5);
    Phong *transparent = new Phong(glm::vec3(0.95f), glm::vec3(0.3f), 2.0f);
    Phong *floor = new Phong(glm::vec3(1, 0.5f, 0), glm::vec3(1), 10.0f);
    floor->add(check);

    // set up scene
    Scene scene = Scene(BACKGROUND);
    Mesh *plane = new Mesh(glm::vec3(-10, 2, 0), glm::vec3(0), glm::vec3(15, 5, 1), floor);
    plane->add(glm::vec3(-1, 1, 0), glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0));
    plane->add(glm::vec3(1, -1, 0), glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0));
    // Mesh *cube = new Mesh(glm::vec3(0, 0, 1), glm::vec3(0, 0, glm::radians(15.0f)), glm::vec3(1), reflective);
    // cube->read("resources/cube.ply");
    Sphere *sphere1 = new Sphere(glm::vec3(0, 0, 2.5), 1.25f, transparent);
    Sphere *sphere2 = new Sphere(glm::vec3(-2, 1.5, 1.5), 1.0f, reflective);
    Light *light = new Light(glm::vec3(5, -1, 10), glm::vec3(1), 1);
    scene.add(*sphere1);
    scene.add(*sphere2);
    scene.add(*plane);
    // scene.add(*cube);
    scene.add(*light);

    // set up camera
    Camera camera = Camera(glm::vec3(10, 0, 2.5), glm::vec3(-1, 0, -glm::tan(glm::radians(5.0f))), glm::vec3(0, 0, 1));

    // render
    cout << "rendering..." << endl;
    glm::vec3 *frame = camera.render(HEIGHT, WIDTH, scene);

    // save to ppm
    ofstream file;
    file.open(FILENAME);
    file << "P3 " << WIDTH << " " << HEIGHT << " 255\n";

    // find maximum intensity component
    float maxval = 1;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        if (frame[i].r > maxval) { maxval = frame[i].r; }
        if (frame[i].g > maxval) { maxval = frame[i].g; }
        if (frame[i].b > maxval) { maxval = frame[i].b; }
    }

    // write scaled pixel value
    glm::ivec3 pixel;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        pixel = glm::floor(255.0f * glm::max(glm::vec3(0), frame[i]) / maxval);
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    cout << "saved to " << FILENAME << "." << endl;
    delete[] frame;
    file.close();


}