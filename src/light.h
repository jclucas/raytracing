#include <glm/vec3.hpp>

class Light {

    private:
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    public:
        Light(glm::vec3 position, glm::vec3 color, float intensity);

};