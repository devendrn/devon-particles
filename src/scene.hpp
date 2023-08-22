#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  public:
    glm::vec3 orb_vector;
    glm::mat4 view;
    const float mouse_sensitivity = 0.3f;
    float zoom = 45.0f;
    float radius;

    Camera(float radius = 3.0f): radius(radius) {
      orb_vector = glm::vec3(0, 0, radius);
      updateCameraVectors(0, 0);
    }

    void ProcessMouseMovement(float dx, float dy) {
      dx *= mouse_sensitivity * 0.01f;
      dy *= mouse_sensitivity * 0.01f;

      updateCameraVectors(dx, dy);
    }

    void ProcessMouseScroll(float dy) {
      zoom -= dy;

      // limit zoom
      if (zoom < 1.0f)
        zoom = 1.0f;
      if (zoom > 45.0f)
        zoom = 45.0f;
    }

  private:
    void updateCameraVectors(float dx, float dy) {
      glm::mat4 rot = glm::rotate(glm::mat4(1), -dx, glm::vec3(0, 1, 0));
      orb_vector = rot * glm::vec4(orb_vector, 1);
      rot = glm::rotate(glm::mat4(1), -dy, glm::cross(orb_vector, glm::vec3(0, 1, 0)));
      orb_vector = rot * glm::vec4(orb_vector, 1);
      view = glm::lookAt(orb_vector, glm::vec3(0), glm::vec3(0, 1, 0));
    }
};

#endif
