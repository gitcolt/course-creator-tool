#include "camera.h"
#include "math_utils.h"
#include "gl_utils.h"
#include "input.h"
#include "command.h"
#include "spline_window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
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
static const unsigned WINDOW_WIDTH = 1280;
static const unsigned WINDOW_HEIGHT = 720;

static const std::vector<float> vertices = {
  //   x      y        z       u     v  
  -100.0f,  0.0f, -100.0f,   0.0f, 1.0f,
  -100.0f,  0.0f,  100.0f,   0.0f, 0.0f,
   100.0f,  0.0f, -100.0f,   1.0f, 1.0f,

   100.0f,  0.0f, -100.0f,   1.0f, 1.0f,
  -100.0f,  0.0f,  100.0f,   0.0f, 0.0f,
   100.0f,  0.0f,  100.0f,   1.0f, 0.0f,
};

static Camera camera{glm::vec3{0.0f, 6.0f, 0.0f}};

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
  //m = glm::rotate(glm::mat4{1.0f}, glm::radians(t * 30.0f), glm::vec3(0.0f, 1.0f, 1.0f));
  return p * camera.get_view_matrix() * m;
}

int main() {
  CameraTranslateCommand cmd_camera_forward{camera, CAMERA_FORWARD};
  CameraTranslateCommand cmd_camera_backward{camera, CAMERA_BACKWARD};
  CameraTranslateCommand cmd_camera_left{camera, CAMERA_LEFT};
  CameraTranslateCommand cmd_camera_right{camera, CAMERA_RIGHT};
  CameraTranslateCommand cmd_camera_up{camera, CAMERA_UP};
  CameraTranslateCommand cmd_camera_down{camera, CAMERA_DOWN};
  CameraRotateCommand cmd_camera_yaw_left{camera, CAMERA_YAW_LEFT};
  CameraRotateCommand cmd_camera_yaw_right{camera, CAMERA_YAW_RIGHT};
  CameraRotateCommand cmd_camera_pitch_up{camera, CAMERA_PITCH_UP};
  CameraRotateCommand cmd_camera_pitch_down{camera, CAMERA_PITCH_DOWN};
  input.register_command(KEY_W, cmd_camera_forward);
  input.register_command(KEY_A, cmd_camera_left);
  input.register_command(KEY_S, cmd_camera_backward);
  input.register_command(KEY_D, cmd_camera_right);
  input.register_command(KEY_E, cmd_camera_up);
  input.register_command(KEY_Q, cmd_camera_down);
  input.register_command(KEY_UP, cmd_camera_pitch_up);
  input.register_command(KEY_DOWN, cmd_camera_pitch_down);
  input.register_command(KEY_LEFT, cmd_camera_yaw_left);
  input.register_command(KEY_RIGHT, cmd_camera_yaw_right);

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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO(); (void)io;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  bool show_course_path_window = true;

  Program prog{vertex_shader_source, fragment_shader_source};
  prog.add_attrib("a_pos", 3, GL_FLOAT);
  prog.add_attrib("a_uv", 2, GL_FLOAT);
  prog.add_uniform("mvp", GL_FLOAT_MAT4);

  Buffer<float> vert_buf{prog, vertices};
  vert_buf.load_texture("grass.jpg");

  glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
  glm::mat4 mvp = p * v * m;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    float t = glfwGetTime();
    input.process_commands();
    mvp = calculate_mvp(t);

		glUseProgram(prog.gl_program);
    prog.set_uniform("mvp", (const GLfloat *)&mvp);
		glDrawArrays(GL_TRIANGLES, 0, vert_buf.total_element_count);

    if (show_course_path_window) {
      show_spline_window(io);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glfwSwapBuffers(window);
	}

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
