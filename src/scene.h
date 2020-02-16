#include <vector>
#include <glm/mat4x4.hpp>

class Object;
class Light;

using namespace std;

class Scene {

    private:
        vector<Light*> lights;
        vector<Object*> objects;
        glm::vec3 background;
    public:
        Scene();
        Scene(glm::vec3 background);
        vector<Light*>* getLights();
        void transform(glm::mat4 m);
        void add(Light& light);
        void add(Object& object);
        void remove(Object& object);
        glm::vec3 cast(glm::vec3 origin, glm::vec3 direction);

};