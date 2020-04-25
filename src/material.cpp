#include <glm/geometric.hpp>

#include "material.h"
#include "light.h"
#include "texture.h"

/**
 * Return coefficient of diffuse reflectance.
 */
float Material::getDiffuse() {
    return diffuse;
}

/**
 * Return coefficient of specular reflectance.
 */
float Material::getSpecular() {
    return specular;
}

/**
 * Return coefficient of transmittance.
 */
float Material::getTransmittance() {
    return transmittance;
}

/**
 * Return index of refraction.
 */
float Material::getIOR() {
    return ior;
}

/** 
 * Set coefficient of diffuse reflectance.
 */
void Material::setDiffuse(float k) {
    this->diffuse = k;
}

/** 
 * Set coefficient of specular reflectance.
 */
void Material::setSpecular(float k) {
    this->specular = k;
}

/** 
 * Set coefficient of transmittance.
 */
void Material::setTransmittance(float k) {
    this->transmittance = k;
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

/**
 * Create a Phong material.
 * @param diffuse reflectance of diffuse illumination
 * @param specular reflectance of specular illumination
 * @param sharpness specular exponent
 */
Phong::Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness) {
    this->diffuse = diffuse;
    this->specular = specular;
    this->sharpness = sharpness;
    this->textures = nullptr;
}

/**
 * Get Phong shading for a point with given geometry.
 * @param p point on surface
 * @param n normal vector
 * @param s direction to source
 * @param r perfectly reflective direction
 * @param v direction to viewer
 * @return intensity at point
 */
glm::vec3 Phong::getColor(glm::vec3 p, glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, Light &light) {

    glm::vec3 diffuse = this->diffuse;

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

    return light.getRadiance() * (diffuse * glm::max(glm::dot(s, n), 0.0f) + specular * powf(glm::max(glm::dot(r, v), 0.0f), sharpness));

}