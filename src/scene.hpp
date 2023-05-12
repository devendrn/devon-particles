#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

// A camera with barely any feature 
class Camera
{
public:
    glm::vec3 orb_vector;
    glm::mat4 view;
    float zoom = 45.0f;
    float mouse_sensitivity = 0.3f;
    float moom;
    float radius;

    Camera(float radius = 3.0f): radius(radius)
    {
        orb_vector = glm::vec3(0, 0, radius);
        view = glm::translate(glm::mat4(1), orb_vector);
    }

    void ProcessMouseMovement(float dx, float dy)
    {
        dx *= mouse_sensitivity*0.01f;
        dy *= mouse_sensitivity*0.01f;
        
        updateCameraVectors(dx, dy);
    }

    void ProcessMouseScroll(float dy)
    {
        zoom -= dy;
        
        // limit zoom
        if (zoom<1.0f)
            zoom=1.0f;
        if (zoom>45.0f)
            zoom=45.0f;
    }

private:
    void updateCameraVectors(float dx, float dy)
    {
        glm::mat4 rot = glm::rotate(glm::mat4(1),-dx,glm::vec3(0, 1, 0));
        orb_vector = rot * glm::vec4(orb_vector,1.0f);
        view = glm::lookAt(orb_vector,glm::vec3(0.0f),glm::vec3(0, 1, 0));
        //view = glm::rotate(view, dy, glm::vec3(1, 0, 0));
    }
};

#endif
