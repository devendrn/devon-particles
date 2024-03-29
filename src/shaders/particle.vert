#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint id;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  
  // 3 particles types (0, 1, 2)
  // color them with hue from green to red
  float a = id/2.0;
  ourColor = hsv2rgb(vec3(0.25 + 0.75*a,0.8,0.5 + 0.5*a));
}
