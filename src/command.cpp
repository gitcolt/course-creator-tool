#include "command.h"
#include "camera.h"

#include <iostream>

CameraTranslateCommand::CameraTranslateCommand(Camera &camera,
                                               enum CameraTranslationType trans_type) :
                                               camera{camera},
                                               trans_type{trans_type} {}

void CameraTranslateCommand::execute() {
  camera.translate(trans_type);
}

CameraRotateCommand::CameraRotateCommand(Camera &camera,
                                         enum CameraRotationType rot_type) :
                                         camera{camera},
                                         rot_type{rot_type} {}

void CameraRotateCommand::execute() {
  camera.rotate(rot_type);
}
