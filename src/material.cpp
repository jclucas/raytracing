#include <glm/geometric.hpp>

#include "material.h"
#include "light.h"

using namespace std;

ostream& Material::print(ostream &s) const {
    return s << "Material\n";
}

/**
 * Output stream operator override.
 */
ostream& operator<<(ostream &s, Material *m) {
    return m->print(s);
}

/**
 * Create a Phong material.
 * @param diffuse reflectivity of diffuse illumination
 * @param specular reflectivity of specular illumination
 * @param sharpness specular exponent
 */
Phong::Phong(glm::vec3 diffuse, glm::vec3 specular, float sharpness) {
    this->diffuse = diffuse;
    this->specular = specular;
    this->sharpness = sharpness;
}

/**
 * Get Phong shading for a point with given geometry.
 * @param n normal vector
 * @param s direction to source
 * @param r perfectly reflective direction
 * @param v direction to viewer
 * @return intensity at point
 */
glm::vec3 Phong::getColor(glm::vec3 n, glm::vec3 s, glm::vec3 r, glm::vec3 v, Light &light) {
    return light.getRadiance() * (diffuse * glm::dot(s, n) + specular * powf(glm::max(glm::dot(r, v), 0.0f), sharpness));
}

/**
 * Print function for output stream operator.
 */
ostream& Phong::print(ostream &s) const {
    return s << "Phong material\n"
        << "diffuse: (" << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << ")\n"
        << "specular: (" << specular.r << ", " << specular.g << ", " << specular.b << ")\n"
        << "sharpness: " << sharpness << endl;
}