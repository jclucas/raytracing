#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Object {

    protected:
        glm::vec3 position;
        glm::vec3 orientation;

        // just a rgb color for now
        glm::vec3 material;

    public:
        virtual void transform(glm::mat4 m);
        virtual bool intersect(glm::vec3 origin, glm::vec3 direction) = 0;
        glm::vec3 getColor();

};

class Sphere : public Object {

    private:
        float radius;
    
    public:
        Sphere(glm::vec3 position, float radius, glm::vec3 material);
        void transform(glm::mat4 m);// : Object::transform(m);
        bool intersect(glm::vec3 origin, glm::vec3 direction);

};

class Plane : public Object {

    private:
        float x;
        float y;

    public:
        Plane(glm::vec3 position, float x, float y, glm::vec3 normal, glm::vec3 material);
        bool intersect(glm::vec3 origin, glm::vec3 direction);

};