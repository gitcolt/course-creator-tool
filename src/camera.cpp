#include "camera.h"
#include "math_utils.h"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch) :
  pos(pos),
  front(glm::vec3(0.0f, 0.0f, -1.0f)),
  world_up(up),
  yaw(yaw),
  pitch(pitch) {
  update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix() const {
  return glm::lookAt(pos, pos + front, up);
}

void Camera::translate(enum CameraTranslationType trans_type) {
  float amt = 1.0f;
  if (trans_type == CAMERA_FORWARD)
    pos += front * amt;
  if (trans_type == CAMERA_BACKWARD)
    pos -= front * amt;
  if (trans_type == CAMERA_LEFT)
    pos -= right * amt;
  if (trans_type == CAMERA_RIGHT)
    pos += right * amt;
  if (trans_type == CAMERA_UP)
    pos.y += amt;
  if (trans_type == CAMERA_DOWN)
    pos.y -= amt;

  update_camera_vectors();
}

void Camera::rotate(enum CameraRotationType rot_type) {
  float amt = 5.0;
  if (rot_type == CAMERA_PITCH_UP)
    pitch += amt;
  if (rot_type == CAMERA_PITCH_DOWN)
    pitch -= amt;
  if (rot_type == CAMERA_YAW_LEFT)
    yaw -= amt;
  if (rot_type == CAMERA_YAW_RIGHT)
    yaw += amt;

  update_camera_vectors();
}

void Camera::update_camera_vectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  this->front = glm::normalize(front);
  right = glm::normalize(glm::cross(this->front, world_up));
  up = glm::normalize(glm::cross(right, this->front));
}

