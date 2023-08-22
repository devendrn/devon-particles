#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  ourColor = vec3(0.7)*( 0.5 + 0.5*(0.5 + aPos));

  // let the quadtree plane have darker col
  if(aPos.z==0.0)
    ourColor = 0.5*normalize(ourColor);
}
