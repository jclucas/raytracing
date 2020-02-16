#include <ostream>
#include <glm/vec3.hpp>

class Light;

using namespace std;

class Material {

    public:
        virtual glm::vec3 getColor(glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, Light &light) = 0;
        virtual ostream& print(ostream &s) const;
        friend ostream& operator<<(ostream &s, Material *m);

};

class Phong : public Material {

    private:
        glm::vec3 diffuse;
        glm::vec3 specular;
        float sharpness;

    public:
        Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness);
        virtual glm::vec3 getColor(glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, Light &light) override;
        virtual ostream& print(ostream &s) const override;
        friend ostream& operator<<(ostream &s, Phong &m);
};