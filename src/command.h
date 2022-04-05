#ifndef COMMAND_H
#define COMMAND_H

#include "camera.h"

#include <string>

struct Command {
    virtual ~Command() {};
    virtual void execute() = 0;
};

class CameraTranslateCommand : public Command {
  public:
    CameraTranslateCommand(Camera &camera, enum CameraMovement dir);
    virtual void execute() override;
  private:
    Camera &camera;
    enum CameraMovement dir;
};

#endif
