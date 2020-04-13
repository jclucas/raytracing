#pragma once

#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "bounding.h"

class Material;
class Scene;
class Primitive;

// for floating point equality cutoffs
#define EPSILON 0.001f

// for recursive rays
#define MAX_DEPTH 5

/**
 * Abstract object class.
 */
class Object {

    protected:
        glm::vec3 position;
        BoundingBox bound;
        Material *material;
        glm::mat4 invWorldMatrix = glm::mat4(1);
        virtual void setBounds() = 0;

    public:
        glm::vec3 getPosition();
        BoundingBox& getBounds();
        glm::vec3 inverseTransform(glm::vec3 p);
        virtual void transform(glm::mat4 m);
        virtual vector<Primitive*>* getPrimitives() = 0;

};

/**
 * A primitive object that can be used for intersection tests.
 */
class Primitive : public Object {

    public:
        virtual float intersect(glm::vec3 origin, glm::vec3 direction) = 0;
        virtual glm::vec3 getNormal(glm::vec3 point) = 0;
        glm::vec3 getColor(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, Scene& scene, int depth);
        virtual vector<Primitive*>* getPrimitives() override;

};

/**
 * Sphere primitive.
 */
class Sphere : public Primitive {

    private:
        float radius;
        virtual void setBounds() override;
    
    public:
        Sphere(glm::vec3 position, float radius, Material *material);
        float intersect(glm::vec3 origin, glm::vec3 direction) override;
        virtual glm::vec3 getNormal(glm::vec3 point) override;

};

/**
 * Triangle primitive.
 */
class Triangle : public Primitive {

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

/**
 * Composite object made of a collection of triangles.
 */
class Mesh : public Object {

    private:
        glm::vec3 rotation;
        glm::vec3 scale;
        vector<Primitive*> components;
        virtual void setBounds() override;
        glm::mat4 getObjectTransform();

    public:
        Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Material* material);
        virtual void transform(glm::mat4 m) override;
        virtual vector<Primitive*>* getPrimitives() override;
        void add(glm::vec3 a, glm::vec3 b, glm::vec3 c);
        void read(std::string filename);

};