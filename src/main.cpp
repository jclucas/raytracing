#include <fstream>
#include <ctime>
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
    transparent->setTransmittance(0.8);
    transparent->setIOR(0.95);
    Phong *floor = new Phong(glm::vec3(1, 0.5f, 0), glm::vec3(1), 10.0f);
    floor->add(check);

    // set up scene
    Scene scene = Scene(BACKGROUND);
    Mesh *plane = new Mesh(glm::vec3(-10, 2, 0), glm::vec3(0), glm::vec3(15, 5, 1), floor);
    plane->add(glm::vec3(-1, 1, 0), glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0));
    plane->add(glm::vec3(1, -1, 0), glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0));
    // Mesh *cube = new Mesh(glm::vec3(0, 0, 1), glm::vec3(0, 0, glm::radians(30.0f)), glm::vec3(1), transparent);
    // cube->read("resources/cube.ply");
    Sphere *sphere1 = new Sphere(glm::vec3(0, 0, 2.5), 1.25f, transparent);
    Sphere *sphere2 = new Sphere(glm::vec3(-2, 1.5, 1.5), 1.0f, reflective);
    Light *light = new Light(glm::vec3(5, -1, 10), glm::vec3(1), 1);
    scene.add(*sphere1);
    scene.add(*sphere2);
    scene.add(*plane);
    // scene.add(*cube);
    scene.add(*light);

    // ALT SCENE (kd tree test)
    // Scene scene = Scene(glm::vec3(1, 1, .75f));
    // Light *light = new Light(glm::vec3(5, -1, 10), glm::vec3(1), 1);
    // Phong *phong = new Phong(glm::vec3(.5f, .5f, 1), glm::vec3(1), 10.0f);
    // Mesh *bunny = new Mesh(glm::vec3(0, 0.5f, -3), glm::vec3(glm::radians(90.0f), glm::radians(90.0f), 0), glm::vec3(30), phong);
    // bunny->read("resources/bun_zipper_res4.ply");
    // // bunny->read("resources/box.ply");
    // scene.add(*bunny);
    // scene.add(*light);

    // set up camera
    Camera camera = Camera(glm::vec3(10, 0, 2.5), glm::vec3(-1, 0, -glm::tan(glm::radians(5.0f))), glm::vec3(0, 0, 1), new WardModel);

    // start clock
    std::clock_t start = std::clock();

    // render
    glm::vec3 *frame = camera.render(HEIGHT, WIDTH, scene);

    // report render time
    double duration = std::difftime(std::clock(), start) / (double) CLOCKS_PER_SEC;
    cout << "finished rendering after " << duration << " seconds." << endl;

    // save to ppm
    ofstream file;
    file.open(FILENAME);
    file << "P3 " << WIDTH << " " << HEIGHT << " 255" << endl;

    // write discrete pixel value
    glm::ivec3 pixel;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        pixel = glm::floor(255.0f * frame[i]);
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    cout << "saved to " << FILENAME << "." << endl;
    delete[] frame;
    file.close();

}