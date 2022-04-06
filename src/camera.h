#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

enum CameraTranslationType {
  CAMERA_FORWARD,
  CAMERA_BACKWARD,
  CAMERA_LEFT,
  CAMERA_RIGHT,
  CAMERA_UP,
  CAMERA_DOWN,
};

enum CameraRotationType {
  CAMERA_PITCH_UP,
  CAMERA_PITCH_DOWN,
  CAMERA_YAW_LEFT,
  CAMERA_YAW_RIGHT,
};

class Camera {
  public:
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 world_up;
    float yaw;
    float pitch;

    Camera(glm::vec3 pos = glm::vec3(0.0f),
          glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
          float yaw = -90.0f,
          float pitch = 0.0f);

    glm::mat4 get_view_matrix() const;
    void translate(enum CameraTranslationType trans_type);
    void rotate(enum CameraRotationType rot_type);

  private:
    void update_camera_vectors();
};

#endif

