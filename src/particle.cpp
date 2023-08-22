#include <glm/gtc/random.hpp>
#include <cstdlib>
#include <vector>

#include "particle.hpp"
#include "quadtree.hpp"

using namespace glm;

ParticleSystem::ParticleSystem() {
  pts_size = 0;
  radius = 0.5f;
  damp = 0.3f;
  effect_dist = 3.0f;
}

bool ParticleSystem::isParticleInBound(float x, float y, float r) {
  float tx = temp_pt -> pos.x;
  float ty = temp_pt -> pos.y;
  if (tx > x - r && tx < x + r && ty > y - r && ty < y + r)
    return true;
  if (abs(x - tx) < effect_dist || abs(y - ty) < effect_dist)
    return true;
  return false;
}

void ParticleSystem::integrateForce(unsigned int id) {
  vec3 disp = pts[id].pos - temp_pt -> pos;

  float dist = length(disp);
  if (0 < dist && dist < effect_dist) {
    float charge = interaction_mat[temp_pt -> id][pts[id].id];
    charge = 1.0f;
    // calculating acc vector
    // very bad
    float dinv = 1.0f / (dist);
    temp_acc += 0.1f * disp * (dinv * dinv * (charge * dinv - radius * dinv * dinv));

    // to visualize quadtree selection points
    if (temp_pt -> id == 3 && pts[id].id != 3) pts[id].id = 2;

    // temp_acc += disp*(-dinv*dinv*radius + 1/radius);
    // if (dist>radius)
    //   temp_acc *= 100*charge*(dinv*dinv - 0.1/(effect_dist*effect_dist)) ;
  }
}

// update particles
void ParticleSystem::step(float delta, std::vector<float> *lines) {
  // using quadtree for optimization
  // assuming square as bound
  Quadtree < ParticleSystem > quadtreePoints(0.0f, 0.0f, bound_size.x, 3, lines);

  // build quadtree
  for (unsigned int i = 0; i < pts_size; i++)
    quadtreePoints.insert(pts[i].pos.x, pts[i].pos.y, i);

  // to visualize quadtree points selection
  for (unsigned int i = 0; i < pts_size; i++)
    pts[i].id = 1;

  // special point to track
  pts[10].id = 3;

  for (unsigned int i = 0; i < pts_size; i++) {
    temp_pt = & pts[i];
    temp_acc = vec3(0.0f);
    quadtreePoints.traverse(this, & ParticleSystem::isParticleInBound, & ParticleSystem::integrateForce);

    //temp_acc.x -= 1000.0f;

    // update velocity
    pts[i].vel += temp_acc * delta - pts[i].vel * delta * damp;

    // clamp velocity magnitude
    float len = length(pts[i].vel);
    if (len > 0) // vector * min( 1 , max_magnitude / magnitude )
      pts[i].vel = pts[i].vel * min(1.0f, 40.0f/len);

    // warp particles to opposte side
    // note: this does not warp force field however
    // therefore the particles seem to be affected by the edges
    if (abs(pts[i].pos.x) > bound_size.x)
      pts[i].pos.x -= 1.999f * sign(pts[i].pos.x) * bound_size.x;
    if (abs(pts[i].pos.y) > bound_size.y)
      pts[i].pos.y -= 1.999f * sign(pts[i].pos.y) * bound_size.y;
    if (abs(pts[i].pos.z) > bound_size.z)
      pts[i].pos.z -= 1.999f * sign(pts[i].pos.z) * bound_size.z;

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
void ParticleSystem::boxFill(vec3 box_size, unsigned int particle_count) {
  for (unsigned int i = 0; i < particle_count; i++) {
    particle pt = {
      // position
      //linearRand(-box_size, box_size),
      vec3(linearRand(vec2(-box_size.x), vec2(box_size.x)), 0.0f),

      // velocity 
      //linearRand(-box_size, box_size),
      vec3(0.0f),

      // id
      // (unsigned char) (rand() % 3)
      (unsigned char) 1
    };
    pts.push_back(pt);
  }
  pts_size = particle_count;
  //pts_size = pts.size();
}

void ParticleSystem::clear() {
  pts.clear();
  pts_size = 0;
}

// set interaction mat with random values
void ParticleSystem::randomizeInteractionMat(float mag) {
  for (unsigned char i = 0; i < 5; i++) {
    for (unsigned char j = 0; j < 5; j++) {
      //interaction_mat[i][j] = mag * ((rand()%2));
      interaction_mat[i][j] = i == j ? mag : 0.0f;
    }
  }
}
