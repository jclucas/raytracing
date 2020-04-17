#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

class KDTree;
class Object;
class Primitive;
class Light;

using namespace std;

typedef struct Hit {
    Primitive *object;
    glm::vec3 point;
} Hit;

class Scene {    

    private:
        vector<Light*> lights;
        vector<Object*> objects;
        glm::vec3 background;
        KDTree* tree;

    public:
        Scene(glm::vec3 background);
        vector<Light*>* getLights();
        vector<Primitive*>* getPrimitives();
        void transform(glm::mat4 m);
        void generateTree(vector<Primitive*>* prims);
        void add(Light& light);
        void add(Object& object);
        Hit cast(glm::vec3 origin, glm::vec3 direction);
        glm::vec3 getPixel(glm::vec3 origin, glm::vec3 direction, int depth);

};
