#include <glm/vec3.hpp>

class Material {

    public:
        virtual glm::vec3 getColor(glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v) = 0;

};

class Phong : public Material {

    private:
        glm::vec3 diffuse;
        glm::vec3 specular;
        float sharpness;

    public:
        Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness);
        virtual glm::vec3 getColor(glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v) override;

};