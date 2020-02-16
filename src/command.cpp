#include <sstream>

#include "command.h"
#include "light.h"

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

    if (type == "light") {
        float a, b, c;
        args >> a >> b >> c;
        glm::vec3 position = glm::vec3(a, b, c);
        args >> a >> b >> c;
        glm::vec3 intensity = glm::vec3(a, b, c);
        args >> a;
        lights[name] = new Light(position, intensity, a);
    } else if (type == "material") {

    } else if (type == "sphere") {
        
    } else if (type == "triangle") {

    } else {
        cout << "Unknown type: " << type << endl;
    }

}