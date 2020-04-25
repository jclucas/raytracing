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
    const int HEIGHT = 720;
    const int WIDTH = 720;
    const std::string FILENAME = "cornell.ppm";

    // scene characteristics
    const glm::vec3 BACKGROUND = glm::vec3(0.1f);

    // create materials
    Phong *white = new Phong(glm::vec3(1.0f), glm::vec3(0), 1);
    white->setDiffuse(1.0f);
    Phong *green = new Phong(glm::vec3(0, 1.0f, 0), glm::vec3(0), 1);
    green->setDiffuse(1.0f);
    Phong *red = new Phong(glm::vec3(1.0f, 0, 0), glm::vec3(0), 1);
    red->setDiffuse(1.0f);
    Phong *reflective = new Phong(glm::vec3(0.8f), glm::vec3(1), 10.0f);
    reflective->setDiffuse(0.5);
    reflective->setSpecular(0.5);
    Phong *transparent = new Phong(glm::vec3(0.95f), glm::vec3(0.3f), 2.0f);
    transparent->setTransmittance(0.8);
    transparent->setIOR(0.95);

    // add lights
    Scene scene = Scene(BACKGROUND);
    Light *light = new Light(glm::vec3(280.0f, 548.7f, 279.5f), glm::vec3(1), 1.0f);
    scene.add(*light);

    //////////////////////////
    // CORNELL BOX
    //////////////////////////
    
    Mesh *ceil = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    ceil->read("resources/cornell/ceiling.ply");
    scene.add(*ceil);
    
    Mesh *leftwall = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), red);
    leftwall->read("resources/cornell/leftwall.ply");
    scene.add(*leftwall);

    Mesh *backwall = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    backwall->read("resources/cornell/backwall.ply");
    scene.add(*backwall);
        
    Mesh *rightwall = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), green);
    rightwall->read("resources/cornell/rightwall.ply");
    scene.add(*rightwall);

    Mesh *floor = new Mesh(glm::vec3(0), glm::vec3(0), glm::vec3(1), white);
    floor->read("resources/cornell/floor.ply");
    scene.add(*floor);

    Sphere *sphere1 = new Sphere(glm::vec3(300, 65, 300), 65.0f, reflective);
    scene.add(*sphere1);
    Sphere *sphere2 = new Sphere(glm::vec3(200, 65, 200), 65.0f, transparent);
    scene.add(*sphere2);

    // set up camera
    Camera camera = Camera(glm::vec3(278, 273, -800), glm::vec3(278, 273, 0), glm::vec3(0, 1, 0));

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