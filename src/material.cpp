#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

#include "material.h"
#include "light.h"
#include "texture.h"

glm::vec3 Material::getRGB(float wavelength) {

    if (wavelength < 380.0 || wavelength > 780.0) {
        return glm::vec3(0);
    } else if(wavelength < 440.0) {
        return glm::vec3(-1.0 * (wavelength - 440.0) / (440.0 - 380.0), 0.0, 1.0);
    } else if (wavelength < 490.0) {
        return glm::vec3(0.0, (wavelength - 440.0) / (490.0 - 440.0), 1.0);
    } else if (wavelength < 510.0) {
        return glm::vec3(0.0, 1.0, -1.0 * (wavelength - 510.0) / (510.0 - 490.0));
    } else if (wavelength < 580.0) {
        return glm::vec3((wavelength - 510.0) / (580.0 - 510.0), 1.0, 0.0);
    } else if (wavelength < 645.0) {
        return glm::vec3(1.0, -1.0 * (wavelength - 645.0) / (645.0 - 580.0), 0.0);
    } else if (wavelength < 780.0) {
        return glm::vec3(1.0, 0.0, 0.0);
    } else {
        return glm::vec3(0);
    }

}

/**
 * Return coefficient of diffuse reflectance.
 */
float Material::getProbDiffuse() {
    return probDiffuse;
}

/**
 * Return coefficient of specular reflectance.
 */
float Material::getProbSpecular() {
    return probSpecular;
}

/**
 * Return coefficient of transmittance.
 */
float Material::getProbTransmit() {
    return probTransmit;
}


float Material::scaleDiffuse(const Photon& incident) {
    return incident.power * glm::dot(getRGB(incident.wavelength), diffuse);
}

float Material::scaleSpecular(const Photon& incident) {
    return incident.power * glm::dot(getRGB(incident.wavelength), specular);
}

float Material::scaleTransmit(const Photon& incident) {
    return incident.power * glm::dot(getRGB(incident.wavelength), transmittance);
}

/**
 * Return index of refraction.
 */
float Material::getIOR(float wavelength) {

    // non-spectral shortcut
    if (wavelength == 0.0f) {
        return ior;
    }

    // add dispersion coefficient
    return ior + (wavelength - 589.3) * disp;

}

/**
 * Add a texture map to the material.
 */
void Material::add(Texture* texture) {

    if (textures == nullptr) {
        textures = new std::vector<Texture*>();
    }

    textures->push_back(texture);

}

void Material::normalizeProbabilities() {
    this->probDiffuse = glm::length(diffuse);
    this->probSpecular = glm::length(specular);
    this->probTransmit = glm::length(transmittance);
    float total = probDiffuse + probSpecular + probTransmit;
    this->probDiffuse /= total;
    this->probSpecular /= total;
    this->probTransmit /= total;
}

/**
 * Create a Phong material.
 * @param diffuse reflectance of diffuse illumination
 * @param specular reflectance of specular illumination
 * @param sharpness specular exponent
 */
Phong::Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness, glm::vec3 transmit, float ior, float disp) {
    this->diffuse = diffuse;
    this->specular = specular;
    this->transmittance = transmit;
    this->ior = ior;
    this->disp = disp;
    this->sharpness = sharpness;
    this->textures = nullptr;
    normalizeProbabilities();
}


/**
 * Get Phong shading for a point with given geometry.
 * @param p point on surface
 * @param n normal vector
 * @param s direction to source
 * @return intensity at point
 */
glm::vec3 Phong::getDiffuse(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 radiance) {

    if (textures != nullptr) {

        for (auto it = textures->begin(); it != textures->end(); it++) {

            switch((*it)->type) {

                case DIFFUSE:
                    // TODO: scale 
                    diffuse = (*it)->getValue(p.x, p.y);
                    break;

                case NORMAL:
                    // normal map
                    break;
                    
            }

        }

    }

    return radiance * (diffuse * glm::one_over_pi<float>() * glm::max(glm::dot(s, n), 0.0f));

}

/**
 * Get specular Phong shading for a point with given geometry.
 * @param p point on surface
 * @param n normal vector
 * @param s direction to source
 * @param r perfectly reflective direction
 * @param v direction to viewer
 * @return intensity at point
 */
glm::vec3 Phong::getSpecular(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, glm::vec3 radiance) {
    return radiance * specular * ((sharpness + 2) / glm::two_pi<float>()) * powf(glm::max(glm::dot(r, v), 0.0f), sharpness);
}