#include <sstream>

#include "command.h"
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