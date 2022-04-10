#include "camera.h"
#include "math_utils.h"
#include "gl_utils.h"
#include "input.h"
#include "command.h"

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
  //   x      y        z       r     g     b
  -100.0f,  0.0f, -100.0f,   1.0f, 0.0f, 0.0f,
  -100.0f,  0.0f,  100.0f,   0.0f, 1.0f, 0.0f,
   100.0f,  0.0f, -100.0f,   0.0f, 0.0f, 1.0f,

   100.0f,  0.0f, -100.0f,   0.0f, 0.0f, 1.0f,
  -100.0f,  0.0f,  100.0f,   0.0f, 1.0f, 0.0f,
   100.0f,  0.0f,  100.0f,   1.0f, 0.0f, 0.0f,
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
  prog.add_attrib("a_color", 3, GL_FLOAT);
  prog.add_uniform("mvp", GL_FLOAT_MAT4);

  Buffer<float> vert_buf{prog, vertices};

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
      static ImVector<ImVec2> points;
      static ImVec2 scrolling(0.0f, 0.0f);
      static bool opt_enable_grid = true;
      static bool opt_enable_context_menu = true;
      static bool adding_line = false;

      ImGui::Begin("Spline");
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

      ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
      ImVec2 canvas_size = ImGui::GetContentRegionAvail();
      if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
      if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
      ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y);
      ImDrawList *draw_list = ImGui::GetWindowDrawList();
      draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
      draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

      ImGui::InvisibleButton("canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
      const bool is_hovered = ImGui::IsItemHovered();
      const bool is_active = ImGui::IsItemActive();
      const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y);
      const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

      if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        points.push_back(mouse_pos_in_canvas);
        points.push_back(mouse_pos_in_canvas);
        adding_line = true;
      }
      if (adding_line) {
        points.back() = mouse_pos_in_canvas;
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
          adding_line = false;
      }

      const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
      if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
        scrolling.x += io.MouseDelta.x;
        scrolling.y += io.MouseDelta.y;
      }

      ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
      if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
      if (ImGui::BeginPopup("context")) {
        if (adding_line)
          points.resize(points.size() - 2);
        adding_line = false;
        if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); }
        if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0)) { points.clear(); }
        ImGui::EndPopup();
      }

      draw_list->PushClipRect(canvas_p0, canvas_p1, true);
      if (opt_enable_grid) {
        const float GRID_STEP = 64.0f;
        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_size.x; x += GRID_STEP)
          draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_size.y; y += GRID_STEP)
          draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
      }
      for (int n = 0; n < points.Size; n += 2)
        draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
      draw_list->PopClipRect();

      ImGui::End();
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
