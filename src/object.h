#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "bounding.h"

class Material;
class Scene;

// for floating point equality cutoffs
#define EPSILON 0.0001f

class Object {

    protected:
        glm::vec3 position;
        BoundingBox bound;
        Material *material;
        virtual void setBounds() = 0;

    public:
        glm::vec3 getPosition();
        BoundingBox& getBounds();
        virtual void transform(glm::mat4 m);
        virtual float intersect(glm::vec3 origin, glm::vec3 direction) = 0;
        virtual glm::vec3 getNormal(glm::vec3 point) = 0;
        glm::vec3 getColor(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, Scene& scene);

};

class Sphere : public Object {

    private:
        float radius;
        virtual void setBounds() override;
    
    public:
        Sphere(glm::vec3 position, float radius, Material *material);
        float intersect(glm::vec3 origin, glm::vec3 direction) override;
        virtual glm::vec3 getNormal(glm::vec3 point) override;

};

class Triangle : public Object {

    private:
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;
        virtual void setBounds() override;

    public:
        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material *material);
        void transform(glm::mat4 m) override;
        float intersect(glm::vec3 origin, glm::vec3 direction) override;
        virtual glm::vec3 getNormal(glm::vec3 point) override;

};