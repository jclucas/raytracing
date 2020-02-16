#include <iostream>
#include <fstream>

#include <glm/vec3.hpp>

#include "camera.h"
#include "object.h"
#include "material.h"
#include "light.h"
#include "command.h"

using namespace std;

int main() {

    CommandLine cmd = CommandLine();
    string input;

    while (true) {

        cout << "> ";
        getline(cin, input);

        if (input == "quit") {
            return 0;
        }

        cmd.parse(input);
        
    }

}