#include "spline_window.h"

#include <cmath>

void show_spline_window(ImGuiIO &io) {
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

  ImGui::InvisibleButton("canvas", canvas_size,
      ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
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
  if (is_active &&
      ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
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
    if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0))
      points.resize(points.size() - 2);
    if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0))
      points.clear();
    ImGui::EndPopup();
  }

  draw_list->PushClipRect(canvas_p0, canvas_p1, true);
  if (opt_enable_grid) {
    const float GRID_STEP = 64.0f;
    for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_size.x; x += GRID_STEP)
      draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y),
          ImVec2(canvas_p0.x + x, canvas_p1.y),
          IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_size.y; y += GRID_STEP)
      draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y),
          ImVec2(canvas_p1.x, canvas_p0.y + y),
          IM_COL32(200, 200, 200, 40));
  }
  for (int n = 0; n < points.Size; n += 2) {
    draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), 
        ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), 
        IM_COL32(255, 255, 0, 255), 2.0f);
    draw_list->AddCircleFilled(ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), 6.0f, IM_COL32(255, 0, 0, 255));
  }
  draw_list->PopClipRect();

  ImGui::End();
}
