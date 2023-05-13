#ifndef PARTICLES_H
#define PARTICLES_H

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <vector>

struct particle
{
    glm::vec3 pos;
    glm::vec3 vel;
    unsigned char id;
};

class ParticleSystem
{
public:
    ParticleSystem();

    glm::vec3 bound_size;
    std::vector<particle> pts;
    unsigned int pts_size;
    unsigned int interaction_mat[10][10];
    float damp;
    float radius;
    float effect_dist;

    glm::vec3 temp_acc;
    particle *temp_pt;

    void setBound(glm::vec3 box_size);

    void step(float delta, std::vector<float> *lines);

    void boxFill(glm::vec3 box_size, unsigned int particle_count);

    void randomizeInteractionMat(float mag);

    bool isParticleInBound(float x, float y, float r);
    void integrateForce(unsigned int id);
    
    void clear();
};

#endif
