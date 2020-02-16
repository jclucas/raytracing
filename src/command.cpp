#include <sstream>
#include <fstream>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include "command.h"
#include "camera.h"
#include "light.h"
#include "material.h"
#include "object.h"

using namespace std;

CommandLine::CommandLine() {
    this->scene = Scene();
}

/**
 * Parse a command line input.
 */
void CommandLine::parse(string& input) {

    string cmd;
    stringstream args = stringstream(input);
    args >> cmd;

    if (cmd == "help") {
        printHelp();
    } else if (cmd == "list") {
        list();
    } else if (cmd == "add") {
        add(args);
    } else if (cmd == "render") {
        render(args);
    } else {
        cout << "Command not recognized: " << input << endl;
    }

}

/** 
 * Print help text.
 */
void CommandLine::printHelp() {
    cout << "Good luck" << endl;
}

/** 
 * List scene elements.
 */
void CommandLine::list() {

    cout << "\nLights: \n\n";

    for (auto i = lights.begin(); i != lights.end(); i++) {
        cout << i->first << ":\n" << *(i->second) << "\n";
    }

    cout << "Materials: \n\n";

    for (auto i = materials.begin(); i != materials.end(); i++) {
        cout << i->first << ":\n" << i->second << "\n";
    }
    
    cout << "Objects: \n\n";

    for (auto i = objects.begin(); i != objects.end(); i++) {
        cout << i->first << ":\n" << i->second << "\n";
    }

}

/**
 * Add an element to the scene.
 */
void CommandLine::add(stringstream& args) {
    
    string type;
    string name;
    args >> type >> name;

    cout << "type: " << type << "\n";
    cout << "name: " << name << "\n";
    
    // temp vars for reading vectors
    float a, b, c;
    
    if (type == "light") {
        
        args >> a >> b >> c;
        glm::vec3 position = glm::vec3(a, b, c);
        args >> a >> b >> c;
        glm::vec3 intensity = glm::vec3(a, b, c);
        args >> a;
        lights[name] = new Light(position, intensity, a);

    } else if (type == "material") {

        args >> a >> b >> c;
        glm::vec3 diffuse = glm::vec3(a, b, c);
        args >> a >> b >> c;
        glm::vec3 specular = glm::vec3(a, b, c);
        args >> a;

        materials[name] = new Phong(diffuse, specular, a);

    } else if (type == "sphere") {

        args >> a >> b >> c;
        glm::vec3 position = glm::vec3(a, b, c);
        args >> a;

        string mat;
        args >> mat;
        
        try {
            materials[mat];
        }
        catch (const std::out_of_range& e) {
            cout << "Material not found: " << mat << endl;
            return;
        }

        objects[name] = new Sphere(position, a, materials[mat]);
        
    } else if (type == "triangle") {

        args >> a >> b >> c;
        glm::vec3 p1 = glm::vec3(a, b, c);
        args >> a >> b >> c;
        glm::vec3 p2 = glm::vec3(a, b, c);
        args >> a >> b >> c;
        glm::vec3 p3 = glm::vec3(a, b, c);

        string mat;
        args >> mat;

        try {
            materials[mat];
        }
        catch (const std::out_of_range& e) {
            cout << "Material not found: " << mat << endl;
            return;
        }

        objects[name] = new Triangle(p1, p2, p3, materials[mat]);

    } else {
        cout << "Unknown type: " << type << endl;
    }

}

void CommandLine::render(stringstream& args) {

    int height, width;
    args >> height >> width;
    
    // set up camera
    Camera camera = Camera(glm::vec3(10, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));

    // add lights
    for (auto i = lights.begin(); i != lights.end(); i++) {
        scene.add(*(i->second));
    }

    // add objects
    for (auto i = objects.begin(); i != objects.end(); i++) {
        scene.add(*(i->second));
    }

    // render
    glm::vec3 *frame = camera.render(height, width, scene);

    // save to ppm
    ofstream file;
    file.open("render.ppm");
    file << "P3 " << width << " " << height << " 255\n";

    // find maximum intensity component
    float maxval = 1;
    for (int i = 0; i < height * width; i++) {
        if (frame[i].r > maxval) { maxval = frame[i].r; }
        if (frame[i].g > maxval) { maxval = frame[i].g; }
        if (frame[i].b > maxval) { maxval = frame[i].b; }
    }

    // write scaled pixel value
    glm::ivec3 pixel;
    for (int i = 0; i < height * width; i++) {
        pixel = glm::floor(255.0f * frame[i] / maxval);
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    delete[] frame;
    file.close();
}