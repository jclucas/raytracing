class Object {

    protected:
        glm::vec3 position;
        glm::vec3 orientation;

        // just a rgb color for now
        glm::vec3 material;

    public:
        virtual bool intersect(glm::vec3 origin, glm::vec3 direction) = 0;
        glm::vec3 getColor();

};

class Sphere : public Object {

    private:
        float radius;
    
    public:
        Sphere(glm::vec3 position, float radius, glm::vec3 material);
        bool intersect(glm::vec3 origin, glm::vec3 direction);

};