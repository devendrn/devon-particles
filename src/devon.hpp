#ifndef DEVON_H
#define DEVON_H

#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

#endif
