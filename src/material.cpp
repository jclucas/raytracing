#include <glm/geometric.hpp>

#include "material.h"
#include "light.h"
#include "texture.h"

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


glm::vec3 Material::scaleDiffuse(glm::vec3 incident) {
    return incident * diffuse;
}

glm::vec3 Material::scaleSpecular(glm::vec3 incident) {
    return incident * specular;
}

glm::vec3 Material::scaleTransmit(glm::vec3 incident) {
    return incident * transmittance;
}

/**
 * Return index of refraction.
 */
float Material::getIOR() {
    return ior;
}

/** 
 * Set index of refraction.
 */
void Material::setIOR(float k) {
    this->ior = k;
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
Phong::Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness, glm::vec3 transmit, float ior) {
    this->diffuse = diffuse;
    this->specular = specular;
    this->transmittance = transmit;
    this->ior = ior;
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

    return radiance * (diffuse * glm::max(glm::dot(s, n), 0.0f));

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
    return radiance * specular * powf(glm::max(glm::dot(r, v), 0.0f), sharpness);
}