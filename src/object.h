class Object {

    private:
        glm::vec3 position;
        glm::vec3 orientation;

        // just a rgb color for now
        glm::vec3 material;

    public:
        glm::vec3 intersect(glm::vec3 origin, glm::vec3 direction);

};