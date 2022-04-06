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
    CameraTranslateCommand(Camera &camera, enum CameraTranslationType trans_type);
    virtual void execute() override;
  private:
    Camera &camera;
    enum CameraTranslationType trans_type;
};

class CameraRotateCommand : public Command {
  public:
    CameraRotateCommand(Camera &camera, enum CameraRotationType rot_type);
    virtual void execute() override;
  private:
    Camera &camera;
    enum CameraRotationType rot_type;
};


#endif
