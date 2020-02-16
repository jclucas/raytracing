#include <iostream>

#include "scene.h"

using namespace std;

class CommandLine {

    private:
        Scene scene;
        void list();
        void add(string& input);

    public:
        CommandLine();
        void printHelp();
        void parse(string& input);

};