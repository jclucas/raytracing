#include <vector>
#include <glm/mat4x4.hpp>

class Object;
class Light;

using namespace std;

class Scene {
    
    typedef struct Hit {
        Object *object;
        glm::vec3 point;
    } Hit;

    private:
        vector<Light*> lights;
        vector<Object*> objects;
        glm::vec3 background;
    public:
        Scene(glm::vec3 background);
        vector<Light*>* getLights();
        void transform(glm::mat4 m);
        void add(Light& light);
        void add(Object& object);
        Hit cast(glm::vec3 origin, glm::vec3 direction);
        glm::vec3 getPixel(glm::vec3 origin, glm::vec3 direction);

};
