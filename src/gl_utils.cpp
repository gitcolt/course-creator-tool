#include "gl_utils.h"

#include <glad/glad.h>

#include <cstdio>
#include <vector>
#include <string>

static void check_shader_compile_errors(GLuint shader, enum ShaderType type) {
  GLint success;
  GLchar info_log[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, info_log);
    printf("ERROR::SHADER_COMPILATION_ERROR of type: %d\n%s\n", type, info_log);
  }
}

void check_prog_link_errors(GLuint program) {
  GLint success;
  GLchar info_log[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 1024, NULL, info_log);
    fprintf(stderr, "ERROR::PROGRAM_COMPILATION_ERROR: %s\n", info_log);
  }
}

GLuint compile_shader(const GLchar *shader_source, enum ShaderType type) {
  GLuint shader;
  if (type == VERTEX)
    shader = glCreateShader(GL_VERTEX_SHADER);
  else if (type == FRAGMENT)
    shader = glCreateShader(GL_FRAGMENT_SHADER);
  else {
    fprintf(stderr, "Unknown shader type %u\n", type);
    return 0;
  }
  glShaderSource(shader, 1, &shader_source, NULL);
  glCompileShader(shader);
  check_shader_compile_errors(shader, type);
  return shader;
}

GLsizei get_GL_type_width_b(GLenum type) {
  switch(type) {
    case GL_BYTE:
      return 1;
    case GL_UNSIGNED_BYTE:
      return 1;
    case GL_SHORT:
      return 2;
    case GL_UNSIGNED_SHORT:
      return 2;
    case GL_INT:
      return 4;
    case GL_UNSIGNED_INT:
      return 4;
    case GL_FLOAT:
      return 4;
    case GL_FLOAT_VEC2:
      return 8;
    default:
      fprintf(stderr, "Unknown GL type '%u'\n", type);
      return -1;
  }
}

Program::Program(const char *vert_shader_source, const char *frag_shader_source) {
  gl_program = glCreateProgram();
  GLuint vertex_shader = compile_shader(vert_shader_source, VERTEX);
  GLuint fragment_shader = compile_shader(frag_shader_source, FRAGMENT);
  glAttachShader(gl_program, vertex_shader);
  glAttachShader(gl_program, fragment_shader);
  glLinkProgram(gl_program);
  check_prog_link_errors(gl_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void Program::add_attrib(const char *var_name, const unsigned num_components, const GLenum type) {
  const Attrib attrib = {
    .identifier = var_name,
    .location = glGetAttribLocation(gl_program, var_name),
    .num_components = num_components,
    .type = type,
  };
  attribs.push_back(attrib);
  this->num_components += num_components;
  stride += num_components * get_GL_type_width_b(type);
}

void Program::add_uniform(const char *var_name, const GLenum type) {
  const Uniform uniform = {
    .identifier = var_name,
    .type = type,
    .location = glGetUniformLocation(gl_program, var_name),
  };
  uniforms.push_back(uniform);
}
