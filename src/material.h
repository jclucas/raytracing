#pragma once

#include <vector>
#include <glm/vec3.hpp>

class Light;
class Texture;
class Photon;

class Material {

    protected:
        glm::vec3 diffuse, specular, transmittance;
        float probDiffuse, probSpecular, probTransmit;
        float ior = 1;
        float disp = 0.0;
        std::vector<Texture*>* textures;
        void normalizeProbabilities();

    public:
        static glm::vec3 getRGB(float wavelength);
        float getProbDiffuse();
        float getProbSpecular();
        float getProbTransmit();
        float scaleDiffuse(const Photon& incident);
        float scaleSpecular(const Photon& incident);
        float scaleTransmit(const Photon& incident);
        float getIOR(float wavelength);
        virtual glm::vec3 getDiffuse(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 radiance) = 0;
        virtual glm::vec3 getSpecular(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, glm::vec3 radiance) = 0;
        void add(Texture* texture);

};

class Phong : public Material {

    private:
        float sharpness;

    public:
        Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness, glm::vec3 transmit = glm::vec3(0), float ior = 1, float disp = 0);
        virtual glm::vec3 getDiffuse(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 radiance) override;
        virtual glm::vec3 getSpecular(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, glm::vec3 radiance) override;

};