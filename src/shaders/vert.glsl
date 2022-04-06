R"glsl(
#version 330
uniform mat4 mvp;
in vec3 a_color;
in vec3 a_pos;
out vec3 color;

void main() {
  gl_Position = mvp * vec4(a_pos, 1.0);
  color = a_color;
}
)glsl"
