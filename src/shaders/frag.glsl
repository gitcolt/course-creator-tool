R"glsl(
#version 330
in vec2 uv;
out vec4 out_color;

uniform sampler2D sampler;

void main() {
  vec3 color = texture(sampler, uv).rgb;
  out_color = vec4(color, 1.0);
}
)glsl"
