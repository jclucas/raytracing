#include <iostream>
#include <unordered_map>

#include "scene.h"

class Material;

using namespace std;

class CommandLine {

    private:
        Scene scene;
        unordered_map<string, Light*> lights;
        unordered_map<string, Material*> materials;
        unordered_map<string, Object*> objects;
        void list();
        void add(stringstream& args);

    public:
        CommandLine();
        void printHelp();
        void parse(string& input);

};