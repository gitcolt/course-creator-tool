#include "command.h"
#include "camera.h"

#include <iostream>

CameraTranslateCommand::CameraTranslateCommand(Camera &camera,
                                                   enum CameraMovement dir) :
                                                   camera{camera},
                                                   dir{dir} {}

void CameraTranslateCommand::execute() {
  camera.translate(dir);
}
