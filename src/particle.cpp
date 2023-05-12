#include <glm/gtc/random.hpp>

#include <cstdlib>
#include <vector>

#include "particle.hpp"
#include "quadtree.hpp"

using namespace glm;

ParticleSystem::ParticleSystem()
{
    pts_size = 0;
    radius = 0.2f;
    damp = 100.0f;        
    effect_dist = 0.2f;
}

// update particles
void ParticleSystem::step(float delta, std::vector<float> *lines)
{
    // using quadtree for optimization
    // assuming cube as bound
    Quadtree quadtreePoints({0.0f,0.0f,bound_size.x},4,lines);

    // build the quadtree - should be moved inside quadtree class
    for (unsigned int i=0; i<pts_size; i++)
        quadtreePoints.insert(pts[i].pos.x,pts[i].pos.y,i);

    for (unsigned int i=0; i<pts_size; i++)
    {
        // get closest particles using quadtree
        std::vector<unsigned int> found;
        quadtreePoints.query(pts[i].pos.x,pts[i].pos.y,effect_dist,&found);

        // to find net acceleration due to particle interaction forces
        vec3 acc = vec3(0.0);
        for (unsigned int j=0; j<found.size(); j++)
        {
            // find the distance between the particles
            // and use that to calculate the force field between them
            // assume same mass for all particles
            vec3 disp = pts[found[j]].pos - pts[i].pos;

            float dist = length(disp);
            if (0<dist && dist<effect_dist)
            {
                float charge = interaction_mat[pts[i].id][pts[found[j]].id];

                // calculating acc vector
                // very bad
                float dinv = 1.0f/(dist);
                acc += disp*(dinv*dinv*(charge*dinv - radius*dinv*dinv));
                //float dinv = 1.0f/(dist*dist);
                //float radcube = radius*radius*radius;
                //if (dist<radius)
                //{
                //    acc -= disp*(dinv-dist/radcube);
                //} else {
                //    acc += 1.0f*charge*disp/dist;
                //}
            }
        }

        acc.x -= 1000.0f;
        
        // update velocity
        pts[i].vel += acc * delta - pts[i].vel * delta * damp;

        // clamp velocity magnitude
        // could be made lighter?
        float len = length(pts[i].vel);  
        if (len>0) // vector * min( 1 , max_magnitude / magnitude )
            pts[i].vel = pts[i].vel * min(1.0f, 0.6f/len);

        // warp particles to opposte side
        // note: this does not warp force field however
        // therefore the particles seem to be affected by the edges
        if (abs(pts[i].pos.x)>bound_size.x)
            pts[i].pos.x -= 1.999f*sign(pts[i].pos.x*bound_size.x);
        if (abs(pts[i].pos.y)>bound_size.y)
            pts[i].pos.y -= 1.999f*sign(pts[i].pos.y*bound_size.y);
        if (abs(pts[i].pos.z)>bound_size.z)
            pts[i].pos.z -= 1.999f*sign(pts[i].pos.z*bound_size.z);

        // simple deflection
        // prone to tunneling
//        if (abs(pts[i].pos.x)>bound_size.x)
//            pts[i].vel.x *= -1;
//        if (abs(pts[i].pos.y)>bound_size.y)
//            pts[i].vel.y *= -1;
//        if (abs(pts[i].pos.z)>bound_size.z)
//            pts[i].vel.z *= -1;

        // update position
        pts[i].pos += 0.2f * pts[i].vel * delta;
    }
};

// fill a box region with particles
void ParticleSystem::boxFill(vec3 box_size, unsigned int particle_count)
{
    for (unsigned int i=0; i<particle_count; i++)
    {
        particle pt = 
        {
            // position
            //linearRand(-box_size, box_size),
            vec3(linearRand(vec2(-box_size.x), vec2(box_size.x)), 0.0f),
             
            // velocity 
            //linearRand(-box_size, box_size),
            vec3(0.0f),
            
            // id
            (unsigned char) (rand() % 3)
        };
        pts.push_back(pt);
    }
    pts_size = particle_count;
    //pts_size = pts.size();
}

void ParticleSystem::clear()
{
    pts.clear();
    pts_size = 0;
}

// set interacton mat wiht random values
void ParticleSystem::randomizeInteractionMat(float mag)
{
    for (unsigned char i=0; i<5; i++) 
    {
        for (unsigned char j=0; j<5; j++)
        {
            //interaction_mat[i][j] = i==j ? mag : 0.0f;
            interaction_mat[i][j] = mag * ((rand()%2));
        }
    }
}
