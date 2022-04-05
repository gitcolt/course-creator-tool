#include "camera.h"
#include "math_helpers.h"
#include "gl_helpers.h"
#include "input.h"
#include "command.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <cstdlib>
#include <cstdio>
#include <vector>

static const char *WINDOW_NAME = "Course Creator Tool";
static const unsigned WINDOW_WIDTH = 640;
static const unsigned WINDOW_HEIGHT = 480;

static const std::vector<float> vertices = {
  //  x      y     z       r     g     b
  -0.6f, -0.4f,  0.0f,   1.0f, 0.0f, 0.0f,
   0.6f, -0.4f,  0.0f,   0.0f, 1.0f, 0.0f,
   0.0f,  0.6f,  0.0f,   0.0f, 0.0f, 1.0f,

   1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
  -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
   1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
};

static Camera camera{glm::vec3{0.0f, 1.0f, 5.0f}};

static Input input{};

static float last_mouse_x{WINDOW_WIDTH/2.0f};
static float last_mouse_y{WINDOW_HEIGHT/2.0f};

static const glm::mat4 p = glm::perspective(glm::radians(45.0f), 1.33f, 0.01f, 1000.0f);
static glm::mat4 m{1.0f};

static const GLchar *vertex_shader_source =
  #include "shaders/vert.glsl"
;

static const GLchar *fragment_shader_source =
  #include "shaders/frag.glsl"
;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

#define BIND_GLFW_KEY_TO_INPUT_KEY(triggered_key, glfw_key, input_key) do { \
    if (triggered_key == glfw_key) { \
      if (action == GLFW_PRESS) \
        input.key_down(input_key); \
      else if (action == GLFW_RELEASE) \
        input.key_up(input_key); \
    } \
  } while (0)

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_W, KEY_W);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_A, KEY_A);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_S, KEY_S);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_D, KEY_D);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_E, KEY_E);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_Q, KEY_Q);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_UP, KEY_UP);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_DOWN, KEY_DOWN);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_LEFT, KEY_LEFT);
  BIND_GLFW_KEY_TO_INPUT_KEY(key, GLFW_KEY_RIGHT, KEY_RIGHT);
}

static void mouse_callback(GLFWwindow *window, double x, double y) {
  float pos_x = static_cast<float>(x);
  float pos_y = static_cast<float>(y);

  float delta_x = pos_x - last_mouse_x;
  float delta_y = last_mouse_y - pos_y;

  last_mouse_x = pos_x;
  last_mouse_y = pos_y;
}

glm::mat4 calculate_mvp(const float t) {
  m = glm::rotate(glm::mat4{1.0f}, glm::radians(t * 30.0f), glm::vec3(0.0f, 1.0f, 1.0f));
  return p * camera.get_view_matrix() * m;
}

int main() {
  CameraTranslateCommand cmd_camera_forward{camera, CAMERA_FORWARD};
  CameraTranslateCommand cmd_camera_backward{camera, CAMERA_BACKWARD};
  CameraTranslateCommand cmd_camera_left{camera, CAMERA_LEFT};
  CameraTranslateCommand cmd_camera_right{camera, CAMERA_RIGHT};
  input.register_command(KEY_W, cmd_camera_forward);
  input.register_command(KEY_A, cmd_camera_left);
  input.register_command(KEY_S, cmd_camera_backward);
  input.register_command(KEY_D, cmd_camera_right);

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
	if (!window)
	{
    fprintf(stderr, "Failed to create window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

  Program prog{vertex_shader_source, fragment_shader_source};
  prog.add_attrib("a_pos", 3, GL_FLOAT);
  prog.add_attrib("a_color", 3, GL_FLOAT);
  prog.add_uniform("mvp", GL_FLOAT_MAT4);

  Buffer<float> vert_buf{prog, vertices};

  glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
  glm::mat4 mvp = p * v * m;

	while (!glfwWindowShouldClose(window))
	{
    float t = glfwGetTime();

    input.process_commands();

    mvp = calculate_mvp(t);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(prog.gl_program);
    prog.set_uniform("mvp", (const GLfloat *)&mvp);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
