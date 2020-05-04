#pragma once

#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "bounding.h"
#include "photon.h"

class Material;
class Scene;
class Primitive;
class Photon;

// for floating point equality cutoffs
#define EPSILON 0.00001f

// fraction of normal to move away from surfaces
#define D_N 0.1f

// for recursive rays
#define MAX_DEPTH 5

class Ray {

    public:
        glm::vec3 origin;
        glm::vec3 direction;
        Ray(glm::vec3 origin, glm::vec3 direction);

};

/**
 * Abstract object class.
 */
class Object {

    protected:
        glm::vec3 position;
        BoundingBox bound;
        glm::mat4 invWorldMatrix = glm::mat4(1);
        virtual void setBounds() = 0;

    public:
        Material *material;
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
        virtual bool intersect(BoundingBox& bounds) = 0;
        bool isReflective();
        bool isTransmissive();
        virtual glm::vec3 getNormal(glm::vec3 point) = 0;
        Ray reflect(glm::vec3 p, glm::vec3 direction);
        Ray refract(glm::vec3 p, glm::vec3 direction);
        glm::vec3 getDirectIllumination(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, Scene& scene, int depth);
        void bounce(Photon& photon, glm::vec3 point, glm::vec3 direction, Scene& scene);
        glm::vec3 getDiffuse(glm::vec3 point, glm::vec3 origin, glm::vec3 direction, glm::vec3 radiance);
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
        virtual bool intersect(BoundingBox& bounds) override;
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
        virtual bool intersect(BoundingBox& bounds) override;
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