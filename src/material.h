#pragma once

#include <vector>
#include <ostream>
#include <glm/vec3.hpp>

class Light;
class Texture;

using std::vector;
using std::ostream;

class Material {

    protected:
        float reflectance = 0;
        float transmittance = 0;
        float ior = 1;
        std::vector<Texture*>* textures;
    public:
        float getReflectance();
        float getTransmittance();
        float getIOR();
        void setReflectance(float k);
        void setTransmittance(float k);
        void setIOR(float k);
        virtual glm::vec3 getColor(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, Light &light) = 0;
        void add(Texture* texture);
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
        virtual glm::vec3 getColor(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, Light &light) override;
        virtual ostream& print(ostream &s) const override;
        friend ostream& operator<<(ostream &s, Phong &m);

};