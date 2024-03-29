#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/fwd.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ctime>
#include <iostream>
#include <chrono>

#include "shader.hpp"
#include "particle.hpp"
#include "scene.hpp"
#include "devon.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(40.0f);

glm::vec2 mouse_lastpos = glm::vec2(SCR_WIDTH /2.0f, SCR_HEIGHT/2.0f);
bool first_mouse_click = true;
bool imgui_focus = false;

float delta_time = 0.0f; // time between current frame and last frame
float last_time = 0.0f;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouseCallback(GLFWwindow *window, double xposIn, double yposIn) {
  glm::vec2 pos = glm::vec2(xposIn, yposIn);

  if (first_mouse_click) {
    mouse_lastpos.x = pos.x;
    mouse_lastpos.y = pos.y;
    first_mouse_click = false;
  }

  float dx = pos.x - mouse_lastpos.x;
  float dy = mouse_lastpos.y - pos.y;

  mouse_lastpos.x = pos.x;
  mouse_lastpos.y = pos.y;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE || imgui_focus)
    return;

  camera.ProcessMouseMovement(dx, dy);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float> (yoffset));
}

// for GL_LINE
const float vertices[] = {
// bound cube
// x0  y0  z0  x1  y1  z1
  -1, -1, -1,  1, -1, -1,
  -1,  1, -1,  1,  1, -1,
  -1, -1,  1,  1, -1,  1,
  -1,  1,  1,  1,  1,  1,
  -1, -1, -1, -1, -1,  1,
   1, -1, -1,  1, -1,  1,
  -1,  1, -1, -1,  1,  1,
   1,  1, -1,  1,  1,  1,
  -1, -1, -1, -1,  1, -1,
   1, -1, -1,  1,  1, -1,
  -1, -1,  1, -1,  1,  1,
   1, -1,  1,  1,  1,  1,

// center plane
  -1, -1,  0,  1, -1,  0,
  -1,  1,  0,  1,  1,  0,
  -1, -1,  0, -1,  1,  0,
   1, -1,  0,  1,  1,  0
};

int main() {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  // glfw window creation
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particle Life 3D", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // import shader files and compile them 
  Shader particleShader("shaders/particle.vert", "shaders/particle.frag");
  Shader boxShader("shaders/box.vert", "shaders/box.frag");

  unsigned int VBO, VAO;
  glGenVertexArrays(1, & VAO);
  glGenBuffers(1, & VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // create particle system 
  // and initialize some parameters
  ParticleSystem points;
  points.bound_size = glm::vec3(10.0f);
  points.boxFill(glm::vec3(10.0f), 600);
  points.randomizeInteractionMat(5.0f);

  // Imgui setup
  ImGui::CreateContext();
  ImGuiIO & io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  // render loop
  while (!glfwWindowShouldClose(window)) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    imgui_focus = io.WantCaptureMouse;

    // get delta time between frames 
    float current_time = static_cast<float> (glfwGetTime());
    delta_time = current_time - last_time;
    last_time = current_time;

    // check key input
    processInput(window);

    // get time delay for particle simulation step
    //auto start = std::chrono::system_clock::now();

    // update particles
    // lines vector is used to store quadtree lines
    std::vector<float> lines;
    points.step(delta_time, & lines);

    //auto end = std::chrono::system_clock::now();
    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    // matrices will be passed into shader
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.view;

    // create transformations for particle
    glm::mat4 model = glm::mat4(1);

    //// render

    // background
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    particleShader.use();
    particleShader.setMat4("model", model);
    particleShader.setMat4("projection", projection);
    particleShader.setMat4("view", view);

    // move all points into vbo
    glBindVertexArray(VAO);
    glBufferData(GL_ARRAY_BUFFER, points.pts_size * sizeof(particle), &points.pts[0], GL_STATIC_DRAW);

    // position attribute for points
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particle), (void *)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(particle), (void *)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    // draw all points
    glDrawArrays(GL_POINTS, 0, points.pts_size);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(3.0f);

    boxShader.use();
    boxShader.setMat4("model", model);
    boxShader.setMat4("projection", projection);
    boxShader.setMat4("view", view);

    unsigned int all_lines_size = lines.size() + 96;
    float * all_lines = new float[all_lines_size];

    // grid and bounds
    for (int i = 0; i < 96; i++) 
      all_lines[i] = points.bound_size.x * vertices[i]; // scale bound (assuming cube for now)
    
    // quadtree lines
    for (int i = 0; i < lines.size(); i++) 
      all_lines[i + 96] = lines[i];

    glBindVertexArray(VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (all_lines_size), & all_lines[0], GL_STATIC_DRAW);

    delete[] all_lines;

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0));
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, all_lines_size);

    ImGui::Begin("Devon Particles v0-dev");
    ImGui::Text("fps: %.1f", 1.0f/delta_time);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // optional: de-allocate all resources once they've outlived their purpose:
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  glfwTerminate();
  return 0;
}

