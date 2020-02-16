#include "command.h"

using namespace std;

CommandLine::CommandLine() {
    this->scene = Scene();
}

/**
 * Parse a command line input.
 */
void CommandLine::parse(string& input) {

    if (input == "help") {
        printHelp();
    } else if (input == "list") {
        list();
    } else if (input == "add") {
        add(input);
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

    cout << "Lights: \n";

    cout << "Materials: \n";
    
    cout << "Objects: \n";

}

/**
 * Add an element to the scene.
 */
void CommandLine::add(string& input) {
    return;
}