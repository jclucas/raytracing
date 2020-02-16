#include "light.h"

using namespace std;

Light::Light(glm::vec3 position, glm::vec3 color, float intensity) {
    this->position = position;
    this->color = color;
    this->intensity = intensity;
}

glm::vec3 Light::getPosition() {
    return position;
}

glm::vec3 Light::getRadiance() {
    return color * intensity;
}

/**
 * Output stream operator override.
 */
ostream& operator<<(ostream &s, const Light& l) {
    return s << "position: (" << l.position.x << ", " << l.position.y << ", " << l.position.z << ")\n"
        << "color: (" << l.color.x << ", " << l.color.y << ", " << l.color.z << ")\n"
        << "intensity: " << l.intensity << endl;
}