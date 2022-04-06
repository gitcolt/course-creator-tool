#include <glad/glad.h>

#include <string>
#include <vector>
#include <type_traits>
#include <iostream>
#include <algorithm>

enum ShaderType {
  VERTEX,
  FRAGMENT,
};

GLuint compile_shader(const GLchar *shader_source, enum ShaderType type);

void check_prog_link_errors(GLuint program);

GLsizei get_GL_type_width_b(GLenum type);

struct Attrib {
  const std::string identifier;
  GLint location;
  unsigned num_components;
  GLenum type;
};

struct Uniform {
  const std::string identifier;
  GLenum type; // should this be GLenum?
  GLint location;
};

class Program {
  public:
    Program(const char *vert_shader_source, const char *frag_shader_source);
    void add_attrib(const char *var_name, const unsigned num_components, const GLenum type);

    void add_uniform(const char *var_name, const GLenum type);

    template <typename T>
    void set_uniform(const char *var_name, const T val) {
      auto uniform_it = std::find_if(uniforms.begin(), uniforms.end(),
          [var_name](const auto &u){return u.identifier == std::string{var_name};});
      Uniform uniform = *uniform_it;
      if (uniform_it == uniforms.end())
        fprintf(stderr, "Program has no uniform with identifier \"%s\"\n", var_name);
      switch (uniform.type) {
        case GL_FLOAT:
          glUniform1f(uniform.location, val);
          break;
        case GL_BOOL:
        case GL_INT:
          glUniform1i(uniform.location, val);
          break;
        case GL_UNSIGNED_INT:
          glUniform1ui(uniform.location, val);
          break;
        default:
          fprintf(stderr, "Unknown uniform type '%u'\n", uniform.type);
      }
    }

    template <typename T>
    void set_uniform(const char *var_name, const T *val) {
      auto uniform_it = std::find_if(uniforms.begin(), uniforms.end(),
          [var_name](const auto &u){return u.identifier == std::string{var_name};});
      Uniform uniform = *uniform_it;
      if (uniform_it == uniforms.end())
        fprintf(stderr, "Program has no uniform with identifier \"%s\"\n", var_name);
      switch (uniform.type) {
        case GL_FLOAT_VEC2:
          glUniform2fv(uniform.location, 1, val);
          break;
        case GL_FLOAT_VEC3:
          glUniform3fv(uniform.location, 1, val);
          break;
        case GL_FLOAT_VEC4:
          glUniform4fv(uniform.location, 1, val);
          break;
        case GL_FLOAT_MAT4:
          glUniformMatrix4fv(uniform.location, 1, false, val);
          break;
      }
    }

    std::vector<Attrib> attribs{};
    std::vector<Uniform> uniforms{};
    unsigned stride{0};
    GLuint gl_program;

  private:
    unsigned num_components;
};

template <typename T>
class Buffer {
  public:
    Buffer(const Program &prog, const std::vector<T> &verts) :
        verts{verts},
        total_element_count{(GLsizei)(verts.size() / (prog.stride / sizeof(T)))} {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
      glGenBuffers(1, &gl_buf);
      glBindBuffer(GL_ARRAY_BUFFER, gl_buf);
      glBufferData(GL_ARRAY_BUFFER,
          sizeof(verts[0]) * verts.size(),
          verts.data(),
          GL_STATIC_DRAW);

      size_t offset = 0;
      for (const auto& attrib : prog.attribs) {
        glVertexAttribPointer(attrib.location,
                              attrib.num_components,
                              attrib.type,
                              GL_FALSE,
                              prog.stride,
                              (void *)offset);
        glEnableVertexAttribArray(attrib.location);
        offset += attrib.num_components * get_GL_type_width_b(attrib.type);
      }
    }

    void update(const std::vector<T> &verts) {
      glBindBuffer(GL_ARRAY_BUFFER, gl_buf);
      glBufferData(GL_ARRAY_BUFFER,
          sizeof(T) * verts.size(),
          verts.data(),
          GL_STATIC_DRAW);
      total_element_count = verts.size() / (this->verts.size() / total_element_count);
      this->verts = verts;
    }

    const std::vector<T> &verts;
    GLsizei total_element_count;
    GLuint gl_buf;

  private:
    GLuint vao;
};

