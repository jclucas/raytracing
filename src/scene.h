#include <vector>
#include <glm/mat4x4.hpp>
#include "object.h"

using namespace std;

class Scene {

    private:
        vector<Sphere> objects;
        glm::vec3 background;
    public:
        Scene(glm::vec3 background);
        void transform(glm::mat4 m);
        void add(Sphere object);
        void remove(Sphere object);
        glm::vec3 cast(glm::vec3 origin, glm::vec3 direction);

};