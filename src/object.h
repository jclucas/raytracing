#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Object {

    protected:
        glm::vec3 position;

        // just a rgb color for now
        glm::vec3 material;

    public:
        virtual void transform(glm::mat4 m);
        virtual float intersect(glm::vec3 origin, glm::vec3 direction) = 0;
        glm::vec3 getColor();

};

class Sphere : public Object {

    private:
        float radius;
    
    public:
        Sphere(glm::vec3 position, float radius, glm::vec3 material);
        float intersect(glm::vec3 origin, glm::vec3 direction);

};

class Triangle : public Object {

    private:
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;

    public:
        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 material);
        void transform(glm::mat4 m);
        float intersect(glm::vec3 origin, glm::vec3 direction);

};