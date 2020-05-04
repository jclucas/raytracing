#pragma once

#include <vector>
#include <random>
#include <glm/mat4x4.hpp>

class KDTree;
class Object;
class Primitive;
class Light;
class PhotonMap;

using namespace std;

#define PI 3.141592f

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
        PhotonMap* map;

    public:
        Scene(glm::vec3 background);
        vector<Light*>& getLights();
        vector<Primitive*>* getPrimitives();
        void transform(glm::mat4 m);
        void generateTree(vector<Primitive*>* prims);
        void generatePhotonMap(int numPhotons);
        void add(Light& light);
        void add(Object& object);
        Hit cast(glm::vec3 origin, glm::vec3 direction);
        glm::vec3 getPixel(glm::vec3 origin, glm::vec3 direction, int depth);

        // ?
        std::mt19937 random = std::mt19937();
        std::uniform_real_distribution<float> dist = std::uniform_real_distribution<float>(0, 1.0);

};
