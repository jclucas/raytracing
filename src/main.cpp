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
    const int HEIGHT = 600;
    const int WIDTH = 800;
    const std::string FILENAME = "cornell2.ppm";

    // scene characteristics
    const glm::vec3 BACKGROUND = glm::vec3(0.0f);

    // create materials
    Phong *white = new Phong(glm::vec3(1.0f), glm::vec3(0), 1);
    Phong *green = new Phong(glm::vec3(0, 1.0f, 0), glm::vec3(0), 1);
    Phong *red = new Phong(glm::vec3(1.0f, 0, 0), glm::vec3(0), 1);
    Phong *reflective = new Phong(glm::vec3(0.8f), glm::vec3(1), 10.0f);
    Phong *transparent = new Phong(glm::vec3(0.05f), glm::vec3(0.05f), 2.0f, glm::vec3(0.9), 1.5f, 0.005);
    // Phong *transparent = new Phong(glm::vec3(0.0f), glm::vec3(0.0f), 2.0f, glm::vec3(1), 1.5f, 0.005);

    // add lights
    Scene scene = Scene(BACKGROUND);
    // Light *light = new Light(glm::vec3(20.0f, 11.0f, 20.0f), glm::vec3(1), 80.0f);
    Light *light = new Light(glm::vec3(2.8f, 5.488f, 2.795f), glm::vec3(1), 20.0f);
    scene.add(*light);

    //////////////////////////
    // CORNELL BOX
    //////////////////////////
    
    Mesh *ceil = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    ceil->read("resources/cornell/ceiling_hole.ply");
    scene.add(*ceil);
    
    Mesh *leftwall = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    leftwall->read("resources/cornell/leftwall.ply");
    scene.add(*leftwall);

    Mesh *backwall = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    backwall->read("resources/cornell/backwall.ply");
    scene.add(*backwall);
        
    Mesh *rightwall = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    rightwall->read("resources/cornell/rightwall.ply");
    scene.add(*rightwall);

    Mesh *floor = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    floor->read("resources/cornell/floor.ply");
    scene.add(*floor);

    // Mesh *prism = new Mesh(glm::vec3(4.0, 2.502, 4.0), glm::vec3(0, .6, 0), glm::vec3(1, 2.5, .5), transparent);
    Mesh *prism = new Mesh(glm::vec3(2.75, 3.0, 3.5), glm::vec3(0.4, 0 , glm::radians(90.0f)), glm::vec3(.5, 1.5, .5), transparent);
    prism->read("resources/prism.ply");
    scene.add(*prism);

    // Mesh *bunny = new Mesh(glm::vec3(2.5f, 1.0f, 3.0f), glm::vec3(0, glm::radians(180.0f), 0), glm::vec3(20), transparent);
    // bunny->read("resources/bun_zipper_res4.ply");
    // scene.add(*bunny);

    // Sphere *sphere1 = new Sphere(glm::vec3(4.0, 2.0, 2.5), 1.5f, transparent);
    // scene.add(*sphere1);
    // Sphere *sphere2 = new Sphere(glm::vec3(1.5, 3.0, 4.0), 1.0f, transparent);
    // scene.add(*sphere2);

    // set up camera
    Camera camera = Camera(glm::vec3(2.78, 2.73, -8.00), glm::vec3(2.78, 2.73, 0), glm::vec3(0, 1, 0), new ReinhardModel);
    // Camera camera = Camera(glm::vec3(2.78, 3.73, -3.00), glm::vec3(2.78, 2.0, 0), glm::vec3(0, 1, 0), new ReinhardModel);

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
        pixel = glm::floor(255.0f * glm::clamp(frame[i] / MAX_DISP_LUM, 0.0f, 1.0f));
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    cout << "saved to " << FILENAME << "." << endl;
    delete[] frame;
    file.close();

}