R"glsl(
#version 330
in vec2 a_uv;
in vec3 a_pos;
out vec2 uv;

uniform mat4 mvp;

void main() {
  gl_Position = mvp * vec4(a_pos, 1.0);
  uv = a_uv;
}
)glsl"
