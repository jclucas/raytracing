#include <vector>
#include <glm/mat4x4.hpp>

#include "object.h"
#include "light.h"

using namespace std;

class Scene {

    private:
        vector<Light*> lights;
        vector<Object*> objects;
        glm::vec3 background;
    public:
        Scene(glm::vec3 background);
        void transform(glm::mat4 m);
        void add(Light& light);
        void add(Object& object);
        void remove(Sphere object);
        glm::vec3 cast(glm::vec3 origin, glm::vec3 direction);

};