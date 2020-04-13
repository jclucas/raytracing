#pragma once

#include <ostream>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

using namespace std;

class Light {

    private:
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    public:
        Light(glm::vec3 position, glm::vec3 color, float intensity);
        virtual void transform(glm::mat4 m);
        glm::vec3 getPosition();
        glm::vec3 getRadiance();
        friend ostream& operator<<(ostream &s, const Light &l);

};